#include "Common.h"
#include "PowerIng-main/Power_ing.h"
#include "PowerIng-main/Power_Math.h"
#include "PowerIng-main/resource.h"

#define SERVERPORT 9000
#define BUFSIZE 512

void ReadServer(int retval, SOCKET sock);
DWORD WINAPI CreateLobbyThread(LPVOID arg);
DWORD WINAPI CreateProcessThread(LPVOID arg);
DWORD WINAPI CreateClientThread(LPVOID arg);

HANDLE LobbyFlag[3];

int PlayerCount;

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(server_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;	
	HANDLE ClientThread;
	HANDLE ExitEvent[3];

	// accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(server_sock, (struct sockaddr*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) 
		err_display("accept()");

	// 접속한 클라이언트 정보 출력
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// ClientThread 생성
	ClientThread = CreateThread(NULL, 0, CreateClientThread, NULL, 0, NULL);
		
	// 플레이어 수 증가
	PlayerCount++;

	if (PlayerCount == 3) {
		// 대기 서버 생성
		ReadServer(retval, server_sock);
	}	
}

void ReadServer(int retval, SOCKET sock)
{
	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) 
		err_quit("listen()");
	
	// 쓰레드 생성
	HANDLE LobbyThread;
	HANDLE ProcessThread;
	LobbyThread = CreateThread(NULL, 0, CreateLobbyThread, NULL, 0, NULL);
	ProcessThread = CreateThread(NULL, 0, CreateProcessThread, NULL, 0, NULL);
}

DWORD WINAPI CreateLobbyThread(LPVOID arg)
{
	WaitForMultipleObjects(3, LobbyFlag, TRUE, INFINITE);
	if (PlayerCount == 3) {

	}
}

DWORD WINAPI CreateProcessThread(LPVOID arg)
{

}

DWORD WINAPI CreateClientThread(LPVOID arg)
{

}
