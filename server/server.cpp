#include "Common.h"
#include "Packit.h"
#include "Player.h"
#include "Globals.h"


#define SERVERPORT 9000
#define BUFSIZE    512


LONG clientCnt = 0;
CRITICAL_SECTION cs;
Player players[3];
HANDLE hLobbyThread, hProcessThread;
HANDLE hClientKeyInputEvent[3], hProcessEvent;
int gameState;


int GenerateClientIndex();
DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ProcessThread(LPVOID lpParam);
DWORD WINAPI LobbyThread(LPVOID lpParam);
int RecvC2SPacket(SOCKET clientSock, const char* buffer, int bufferSize, int clientID);
int SendS2CPacket(SOCKET clientSock);


int GenerateClientIndex()
{
	int userID = -1;

	EnterCriticalSection(&cs);
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
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
	SOCKET clientSock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];
	InitializeCriticalSection(&cs);

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID 생성 (0~2사이의 값으로 clients의 인덱스로 사용)
	int clientID = GenerateClientIndex();

	while (1) {
		// 패킷 받기 & 처리
		retval = RecvC2SPacket(clientSock, clientID);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 클라이언트 입력완료
		SetEvent(hClientKeyInputEvent[clientID]);

		// 데이터 처리 대기
		DWORD result = WaitForSingleObject(hProcessEvent, INFINITE);
		
		retval = SendS2CPacket(clientSock);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
	}

	// 소켓 닫기
	closesocket(clientSock);
	// 클라이언트 수 감소
	InterlockedDecrement(&clientCnt);
	// player 비활성화
	players[clientID].DisablePlayer();
	printf("[TCP 서버] 클라이언트 종료: ID=%d, IP 주소=%s, 포트 번호=%d\n",
		clientID, addr, ntohs(clientaddr.sin_port));

	return 0;
}

DWORD WINAPI ProcessThread(LPVOID lpParam)
{
	while (gameState != GAME_STATE_END)
	{
		// 모든 클라이언트의 키입력을 대기
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, TRUE, 300);
		ResetEvent(hProcessEvent);

		if (result == WAIT_FAILED) {
			// .
		}
		else if (result == WAIT_TIMEOUT) {
			// 일정 시간동안 클라이언트의 입력을 받지 못하면 이전 입력값을 사용한다.
			for (int i = 0; i < clientCnt; i++)
			{
				if (WaitForSingleObject(hClientKeyInputEvent[i], 0) != WAIT_OBJECT_0) {
					// action키만 비활성화
					players[i].SetActionKeyDwon(false);
				}
			}
		}
		else {
			// 데이터 처리
			

			// 데이터 처리 완료
			SetEvent(hProcessEvent);
		}
	}
}

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
	// 로비 처리
	while (gameState == GAME_STATE_LOBBY)
	{
		
	}

	// 프로세스 스레드 시작
	ResumeThread(hProcessThread);
}

int main(int argc, char* argv[])
{
	int retval;

	// ProcessThread는 대기상태로 생성 LobbyThread가 종료할 때 스레드를 깨움
	hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
	hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

	// 이벤트 생성
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
		hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hProcessEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

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

	while (clientCnt < MAX_NUM_CLIENTS) {
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

int RecvC2SPacket(SOCKET clientSock, int clientID)
{
	char buf[BUFSIZE + 1];
	int retval = recv(clientSock, buf, BUFSIZE, 0);

	if (retval == SOCKET_ERROR || retval == 0) {
		return retval;
	}
	else if (retval < sizeof(PacketType)) {
		printf("error 패킷 사이즈가 너무 작습니다. %s\n", buf);
		return;
	}

	PacketType type;
	memcpy(&type, buf, sizeof(PacketType));

	switch (type) {
	case PACKET_TYPE_KEY_INPUT: {
		if (retval < sizeof(KeyInputPacket)) {
			return;
		}

		KeyInputPacket keyInputPacket;
		memcpy(&keyInputPacket, buf, sizeof(KeyInputPacket));

		players[clientID].SetKeyInput(keyInputPacket.keyInput);
		/*
		printf("키 인풋 처리, up=%s, down=%s, right=%s, left=%s, action=%s",
			keyInputPacket.keyInput.up ? "true" : "false",
			keyInputPacket.keyInput.down ? "true" : "false",
			keyInputPacket.keyInput.right ? "true" : "false",
			keyInputPacket.keyInput.left ? "true" : "false",
			keyInputPacket.keyInput.action ? "true" : "false");
		*/
		break;
	}
							  // 다른 패킷 타입들 예외처리
	default:
		printf("error 패킷타입이 정의되지 않았습니다.\n");
		break;
	}
}

int SendS2CPacket(SOCKET clientSock)
{
	char buf[BUFSIZE + 1];
	int retval = 0;

	switch (gameState)
	{
	case GAME_STATE_GAME:
		GameDataPacket gameDataPacket;
		// GameDataPacket 채우기
		// ...

		memcpy(buf, &gameDataPacket, sizeof(GameDataPacket));
		retval = send(clientSock, buf, sizeof(gameDataPacket), 0);

		return retval;
		break;
	default:
		break;
	}
}