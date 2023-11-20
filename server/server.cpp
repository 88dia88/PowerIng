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
		// ��Ȱ��ȭ�� �÷��̾ ���ο� Ŭ���̾�Ʈ�� �Ҵ��Ѵ�.
		if (!players[i].GetState()) {
			userID = i;
			// �ʱ�ȭ
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

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ClientThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];
	InitializeCriticalSection(&cs);

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID ���� (0~2������ ������ clients�� �ε����� ���)
	int clientID = GenerateClientIndex();

	while (1) {
		// ������ �ޱ�
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ������ ó��
		buf[retval] = '\0';
		//printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
		ProcessPacket(buf, retval);

		// ������ ������
		/*
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		*/
	}

	// ���� �ݱ�
	closesocket(client_sock);
	// Ŭ���̾�Ʈ �� ����
	InterlockedDecrement(&clientCnt);
	// player ��Ȱ��ȭ
	players[clientID].DisablePlayer();
	printf("[TCP ����] Ŭ���̾�Ʈ ����: ID=%d, IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		clientID, addr, ntohs(clientaddr.sin_port));

	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
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

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { 
			closesocket(client_sock); 
		}
		else { 
			CloseHandle(hThread); 
			InterlockedIncrement(&clientCnt); // Ŭ���̾�Ʈ �� ����
		}
	}

	DeleteCriticalSection(&cs);
	closesocket(listen_sock);
	WSACleanup();

	return 0;
}
