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
	SOCKET clientSock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];
	InitializeCriticalSection(&cs);

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID ���� (0~2������ ������ clients�� �ε����� ���)
	int clientID = GenerateClientIndex();

	while (1) {
		// ��Ŷ �ޱ� & ó��
		retval = RecvC2SPacket(clientSock, clientID);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// Ŭ���̾�Ʈ �Է¿Ϸ�
		SetEvent(hClientKeyInputEvent[clientID]);

		// ������ ó�� ���
		DWORD result = WaitForSingleObject(hProcessEvent, INFINITE);
		
		retval = SendS2CPacket(clientSock);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
	}

	// ���� �ݱ�
	closesocket(clientSock);
	// Ŭ���̾�Ʈ �� ����
	InterlockedDecrement(&clientCnt);
	// player ��Ȱ��ȭ
	players[clientID].DisablePlayer();
	printf("[TCP ����] Ŭ���̾�Ʈ ����: ID=%d, IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		clientID, addr, ntohs(clientaddr.sin_port));

	return 0;
}

DWORD WINAPI ProcessThread(LPVOID lpParam)
{
	while (gameState != GAME_STATE_END)
	{
		// ��� Ŭ���̾�Ʈ�� Ű�Է��� ���
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, TRUE, 300);
		ResetEvent(hProcessEvent);

		if (result == WAIT_FAILED) {
			// .
		}
		else if (result == WAIT_TIMEOUT) {
			// ���� �ð����� Ŭ���̾�Ʈ�� �Է��� ���� ���ϸ� ���� �Է°��� ����Ѵ�.
			for (int i = 0; i < clientCnt; i++)
			{
				if (WaitForSingleObject(hClientKeyInputEvent[i], 0) != WAIT_OBJECT_0) {
					// actionŰ�� ��Ȱ��ȭ
					players[i].SetActionKeyDwon(false);
				}
			}
		}
		else {
			// ������ ó��
			

			// ������ ó�� �Ϸ�
			SetEvent(hProcessEvent);
		}
	}
}

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
	// �κ� ó��
	while (gameState == GAME_STATE_LOBBY)
	{
		
	}

	// ���μ��� ������ ����
	ResumeThread(hProcessThread);
}

int main(int argc, char* argv[])
{
	int retval;

	// ProcessThread�� �����·� ���� LobbyThread�� ������ �� �����带 ����
	hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
	hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

	// �̺�Ʈ ����
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
		hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hProcessEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

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

	while (clientCnt < MAX_NUM_CLIENTS) {
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

int RecvC2SPacket(SOCKET clientSock, int clientID)
{
	char buf[BUFSIZE + 1];
	int retval = recv(clientSock, buf, BUFSIZE, 0);

	if (retval == SOCKET_ERROR || retval == 0) {
		return retval;
	}
	else if (retval < sizeof(PacketType)) {
		printf("error ��Ŷ ����� �ʹ� �۽��ϴ�. %s\n", buf);
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
		printf("Ű ��ǲ ó��, up=%s, down=%s, right=%s, left=%s, action=%s",
			keyInputPacket.keyInput.up ? "true" : "false",
			keyInputPacket.keyInput.down ? "true" : "false",
			keyInputPacket.keyInput.right ? "true" : "false",
			keyInputPacket.keyInput.left ? "true" : "false",
			keyInputPacket.keyInput.action ? "true" : "false");
		*/
		break;
	}
							  // �ٸ� ��Ŷ Ÿ�Ե� ����ó��
	default:
		printf("error ��ŶŸ���� ���ǵ��� �ʾҽ��ϴ�.\n");
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
		// GameDataPacket ä���
		// ...

		memcpy(buf, &gameDataPacket, sizeof(GameDataPacket));
		retval = send(clientSock, buf, sizeof(gameDataPacket), 0);

		return retval;
		break;
	default:
		break;
	}
}