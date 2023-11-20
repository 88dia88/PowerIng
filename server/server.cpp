#include "Common.h"
#include "Packit.h"
#include "Player.h"
#include "Globals.h"

#define SERVERPORT 9000
#define BUFSIZE    512

LONG clientCnt = 0;
CRITICAL_SECTION cs;
Player players[3];

int GenerateClientIndex()
{
	int userID = -1;

	EnterCriticalSection(&cs);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		// 비활성화된 플레이어에 새로운 클라이언트를 할당한다.
		if (!players[i].GetState()) {
			userID = i;
			// 초기화
			players[i].DisablePlayer();
			players[i].SetPlayer(userID);
			break;
		}
	}
	LeaveCriticalSection(&cs);

	if (userID == -1)
		printf("error client index is -1");

	return userID;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ClientThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];
	InitializeCriticalSection(&cs);

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID 생성 (0~2사이의 값으로 clients의 인덱스로 사용)
	int clientID = GenerateClientIndex();

	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 처리
		buf[retval] = '\0';
		//printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
		ProcessPacket(buf, retval);

		// 데이터 보내기
		/*
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		*/
	}

	// 소켓 닫기
	closesocket(client_sock);
	// 클라이언트 수 감소
	InterlockedDecrement(&clientCnt);
	// player 비활성화
	players[clientID].DisablePlayer();
	printf("[TCP 서버] 클라이언트 종료: ID=%d, IP 주소=%s, 포트 번호=%d\n",
		clientID, addr, ntohs(clientaddr.sin_port));

	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (clientCnt < MAX_CLIENTS) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { 
			closesocket(client_sock); 
		}
		else { 
			CloseHandle(hThread); 
			InterlockedIncrement(&clientCnt); // 클라이언트 수 증가
		}
	}

	DeleteCriticalSection(&cs);
	closesocket(listen_sock);
	WSACleanup();

	return 0;
}
