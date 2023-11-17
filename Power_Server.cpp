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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
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

	// ������ ��ſ� ����� ����
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

	// ������ Ŭ���̾�Ʈ ���� ���
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// ClientThread ����
	ClientThread = CreateThread(NULL, 0, CreateClientThread, NULL, 0, NULL);
		
	// �÷��̾� �� ����
	PlayerCount++;

	if (PlayerCount == 3) {
		// ��� ���� ����
		ReadServer(retval, server_sock);
	}	
}

void ReadServer(int retval, SOCKET sock)
{
	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) 
		err_quit("listen()");
	
	// ������ ����
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
