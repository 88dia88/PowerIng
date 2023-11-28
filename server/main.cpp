#include "Common.h"
#include "Packet.h"
#include "GamePlayer.h"
#include "Globals.h"
#include "GameTimer.h"


#define SERVERPORT 9000
#define BUFSIZE    1024


LONG clientCnt = 0;
CRITICAL_SECTION cs;
GamePlayer players[3];
HANDLE hLobbyThread, hProcessThread;
HANDLE hClientKeyInputEvent[3], hProcessEvent[3];
int gameState = GAME_STATE_LOBBY;
GameTimer gameTimer;
SOCKET gClientSock[3];

int GenerateClientIndex();
DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ProcessThread(LPVOID lpParam);
DWORD WINAPI LobbyThread(LPVOID lpParam);
int RecvC2SPacket(SOCKET clientSock, int clientID);
int SendS2CPacket(SOCKET clientSock, int clientID);


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

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID ���� (0~2������ ������ clients�� �ε����� ���)
	int clientID = GenerateClientIndex();
	gClientSock[clientID] = clientSock;

	SendS2CPacket(clientSock, clientID);

	while (1) {
		// ��Ŷ �ޱ�
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
		DWORD result = WaitForSingleObject(hProcessEvent[clientID], INFINITE);

		retval = SendS2CPacket(clientSock, clientID);
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
	gameTimer.Reset();

	while (gameState != GAME_STATE_END)
	{
		// ��� Ŭ���̾�Ʈ�� Ű�Է��� ���
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, TRUE, 300);

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
		// ������ ó��
		gameTimer.Tick();
		// gameloop 

		// ������ ó�� �Ϸ�
		for (int i = 0; i < MAX_NUM_CLIENTS; i++)
		{
			SetEvent(hProcessEvent[i]);
		}
	}

	return 0;
}

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
	gameTimer.Reset();
	
	// �κ� ó��
	while (gameState == GAME_STATE_LOBBY)
	{
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, FALSE, INFINITE);
		gameTimer.Tick();
				
		SetEvent(hProcessEvent[result]);

		if (clientCnt >= 1) {
			break;
		}
	}

	// ���μ��� ������ ����
	gameState = GAME_STATE_READY;
	ResumeThread(hProcessThread);

	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// ProcessThread�� �����·� ���� LobbyThread�� ������ �� �����带 ����
	hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
	hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, CREATE_SUSPENDED, NULL);

	InitializeCriticalSection(&cs);
	// �̺�Ʈ ����
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
	{
		hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL); 
		hProcessEvent[i] = CreateEvent(NULL, FALSE, TRUE, NULL);
	}
	

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
			ResumeThread(hLobbyThread);
		}
	}

	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
	{
		CloseHandle(hClientKeyInputEvent[i]);
		CloseHandle(hProcessEvent[i]);
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
		return retval;
	}

	PacketType type;
	memcpy(&type, buf, sizeof(PacketType));

	switch (type) {
	case PACKET_TYPE_KEY_INPUT: {
		if (retval < sizeof(KeyInputPacket)) {
			printf("Error: ��Ŷ ������ ���� KeyInputPacket.\n");
			return retval;
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
	case PACKET_TYPE_PLAYERS_DATA: {
		if (retval < sizeof(PlayersDataPacket)) {
			printf("Error: ��Ŷ ������ ���� PlayerDataPacket.\n");
			return retval;
		}

		PlayersDataPacket playerDataPacket;
		memcpy(&playerDataPacket, buf, sizeof(PlayersDataPacket));

		players[clientID].SetPlayerData(playerDataPacket.player);
		break;
	}
	case PACKET_TYPE_CLIENT_DATA: {
		if (retval < sizeof(ClientDataPacket)) {
			printf("Error: ��Ŷ ������ ���� ClientDataPacket.\n");
			return retval;
		}

		ClientDataPacket clientDataPacket;
		memcpy(&clientDataPacket, buf, sizeof(ClientDataPacket));

		players[clientID].SetPanel(clientDataPacket.color, clientDataPacket.module);
		printf("clientID: %d - clientColor: %d, clientModule: %d\n", 
			clientID, clientDataPacket.color, clientDataPacket.module);
		break;
	}
	default:
		printf("error ��ŶŸ���� ���ǵ��� �ʾҽ��ϴ�.\n");
		break;
	}

	return retval;
}

int SendS2CPacket(SOCKET clientSock, int clientID)
{
	char buf[BUFSIZE + 1];
	int retval = 0;

	switch (gameState)
	{
		// InGame
	case GAME_STATE_GAME: {
		GameDataPacket gameDataPacket;

		// GameDataPacket ä���
		gameDataPacket.playerCount = clientCnt;
		//gameDataPacket.players = new Player[clientCnt];
		//memcpy(gameDataPacket.players, players, sizeof(Player) * clientCnt);
		// gameDataPacket.ballData = ballData;

		// ������ ����
		send(clientSock, reinterpret_cast<char*>(&gameDataPacket), sizeof(GameDataPacket), 0);

		//delete[] gameDataPacket.players;
		return retval;
		break;
	}

		// InLobby
	case GAME_STATE_LOBBY: {
		LobbyDataPacket lobbyDataPacket;

		// GameDataPacket ä���
		lobbyDataPacket.playerCount = clientCnt;
		lobbyDataPacket.clientID = clientID;

		//lobbyDataPacket.players = new Player[clientCnt];
		//memcpy(lobbyDataPacket.players, players, sizeof(Player) * clientCnt);

		// ������ ����
		send(clientSock, reinterpret_cast<char*>(&lobbyDataPacket), sizeof(LobbyDataPacket), 0);

		//delete[] lobbyDataPacket.players;
		return retval; 
		}

		// ReadyRound
	case GAME_STATE_READY:

		return retval;

	default:
		return 0;
	}
}