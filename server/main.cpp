#include "Common.h"
#include "Packet.h"
#include "GamePlayer.h"
#include "Globals.h"
#include "GameTimer.h"
#include "Power_ing.h"


#define DEBUG
#define SERVERPORT 9000
#define BUFSIZE    10000


LONG clientCnt = 0;
CRITICAL_SECTION cs;
HANDLE hLobbyThread, hProcessThread;
HANDLE hClientKeyInputEvent[3], hProcessEvent[3];
GameState gameState = GAME_STATE_LOBBY;
GameTimer gameTimer;
SOCKET gClientSock[3];

int GenerateClientIndex();
DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ProcessThread(LPVOID lpParam);
DWORD WINAPI LobbyThread(LPVOID lpParam);
int RecvC2SPacket(SOCKET clientSock, int clientID);
int SendS2CPacket(SOCKET clientSock, int clientID);
int SendS2CPacketAllPlayer(PacketType packetType);


int GenerateClientIndex()
{
	int userID = -1;

	EnterCriticalSection(&cs);
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
	{
		// 비활성화된 플레이어에 새로운 클라이언트를 할당한다.
		if (!Player[i].Online) {
			userID = i;
			// 초기화
			Player[i] = PlayerReset(Player[i]);
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

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(clientSock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	//clientID 생성 (0~2사이의 값으로 clients의 인덱스로 사용)
	int clientID = GenerateClientIndex();
	gClientSock[clientID] = clientSock;

	//SendS2CPacket(clientSock, clientID);

	while (1) {
		// 패킷 받기
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
		DWORD result = WaitForSingleObject(hProcessEvent[clientID], INFINITE);

		/*
		retval = SendS2CPacket(clientSock, clientID);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		*/
	}

	// 소켓 닫기
	closesocket(clientSock);
	// 클라이언트 수 감소
	InterlockedDecrement(&clientCnt);
	// player 비활성화
	Player[clientID] = DisablePlayer(Player[clientID]);
	printf("[TCP 서버] 클라이언트 종료: ID=%d, IP 주소=%s, 포트 번호=%d\n",
		clientID, addr, ntohs(clientaddr.sin_port));

	return 0;
}

DWORD WINAPI ProcessThread(LPVOID lpParam)
{
	bool orbLaunch = false;
	double score = 0, Temperture = 0, Mole = 0, TotalScore = 0;
	int time = 0, PreTime = 0, ReactorEffect = 0, orbType = 0, orbcount = 0;
	// setGame
	OrbHead->next = OrbHead;
	GeneralReset();
	Temperture = Kelvin, Mole = MaxMole * 0.5;
	PreTime = -25;
	Orbcount = 0;
	TotalScore = 0;
	GameStart = false;
	//EffectHead->next = EffectHead;
	
	// init player
	

	gameTimer.Reset();

	while (gameState != GAME_STATE_END)
	{
		// 모든 클라이언트의 키입력을 대기
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, TRUE, 300);

		if (result == WAIT_FAILED) {
			// .
		}
		else if (result == WAIT_TIMEOUT) {
			// 일정 시간동안 클라이언트의 입력을 받지 못하면 이전 입력값을 사용한다.
			for (int i = 0; i < clientCnt; i++)
			{
				if (WaitForSingleObject(hClientKeyInputEvent[i], 0) != WAIT_OBJECT_0) {
					// action키만 비활성화
					Player[i].actionKeyDown = false;
					//players[i].SetActionKeyDwon(false);
				}
			}
		}
		//---------------------------------------------------------------------데이터 처리
		gameTimer.Tick();
		// gameloop 
		// 클라이언트 action키 처리 - 게임 시작 & 스킬 발동
		for (int i = 0; i < clientCnt; i++)
			if (Player[i].actionKeyDown) {
				if (gameState == GAME_STATE_READY) {
					orbLaunch = true;
					gameState = GAME_STATE_GAME;
					SendS2CPacketAllPlayer(PACKET_TYPE_CHANGE_GAME_STATE);
				}
				else if (gameState == GAME_STATE_GAME) {
					if (Reactor.cherenkovmeter == 100) 
						Reactor.cherenkov = true;
					//완전 충전 되지 않았으면 꾹 눌러서 발동
					else if (Reactor.cherenkovmeter >= 875) {
						Reactor.cherenkov = true;
					}
				}
				break;
			}

		// 클라이언트 방향키 처리 - 패널 이동
		for (int i = 0; i < clientCnt; i++)
		{
			if (Player[i].Online) {
				Player[i].Reflector = ReflectorPosition(Player[i].Reflector,
					Player[i].leftKeyDown, Player[i].rightKeyDown, Player[i].upKeyDown, Player[i].downKeyDown);

				Player[i].Reflector = ReflectorProcess(Player[i].Reflector, gameState == GAME_STATE_GAME);
			}
		}

		// 종료조건에 만족 하는가?
		if (gameState != GAME_STATE_READY and Reactor.meltdown == false and Orbcount < 0) {
			OrbClear();
			gameState = GAME_STATE_END;
			SendS2CPacketAllPlayer(PACKET_TYPE_CHANGE_GAME_STATE);
		}
		// 게임 진행
		else {
			if (ReactorMeltdown() and gameState == GAME_STATE_GAME) {
				gameState = GAME_STATE_READY;
				OrbClear();
			}
			else {
				ReactorCherenkov();

				// orb 충돌체크
				if (gameState == GAME_STATE_GAME) {
					CollisionDetect(OrbHead);
					if (orbLaunch) {
						orbLaunch = false;
						//ColliderColor = 0;
						OrbCreate(OrbHead, OrbType, true, 0, 0, 0.25);
					}
				}
			}
		}
		Time++;
		// 데이터 처리 완료

		// 데이터 전송
		SendS2CPacketAllPlayer(PACKET_TYPE_IN_GAME);

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
	
	// 로비 처리
	while (gameState == GAME_STATE_LOBBY)
	{
		DWORD result = WaitForMultipleObjects(clientCnt, hClientKeyInputEvent, FALSE, INFINITE);
		gameTimer.Tick();
		
		SendS2CPacketAllPlayer(PACKET_TYPE_LOBBY);

		SetEvent(hProcessEvent[result]);

		if (clientCnt >= 1)
			break;
	}

	// 프로세스 스레드 시작
	gameState = GAME_STATE_READY;
	SendS2CPacketAllPlayer(PACKET_TYPE_CHANGE_GAME_STATE);
	ResumeThread(hProcessThread);

	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// ProcessThread는 대기상태로 생성 LobbyThread가 종료할 때 스레드를 깨움
	hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
	hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, CREATE_SUSPENDED, NULL);

	InitializeCriticalSection(&cs);
	// 이벤트 생성
	for (int i = 0; i < MAX_NUM_CLIENTS; i++)
	{
		hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL); 
		hProcessEvent[i] = CreateEvent(NULL, FALSE, TRUE, NULL);
	}
	

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
			ResumeThread(hLobbyThread);
		}
	}

	WaitForSingleObject(hProcessThread, INFINITE);

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
		printf("error 패킷 사이즈가 너무 작습니다. %s\n", buf);
		return retval;
	}

	PacketType type;
	memcpy(&type, buf, sizeof(PacketType));

	switch (type) {
	case PACKET_TYPE_KEY_INPUT: {
		if (retval < sizeof(KeyInputPacket)) {
			printf("Error: 패킷 사이즈 오류 KeyInputPacket.\n");
			return retval;
		}

		KeyInputPacket keyInputPacket;
		memcpy(&keyInputPacket, buf, sizeof(KeyInputPacket));
		
		//SetKeyInput(keyInputPacket.keyInput);
		Player[clientID].upKeyDown = keyInputPacket.keyInput.up;
		Player[clientID].downKeyDown = keyInputPacket.keyInput.down;
		Player[clientID].rightKeyDown = keyInputPacket.keyInput.right;
		Player[clientID].leftKeyDown = keyInputPacket.keyInput.left;
		Player[clientID].actionKeyDown = keyInputPacket.keyInput.action;

		
		printf("%d client, up=%s, down=%s, right=%s, left=%s, action=%s\n",
			clientID,
			keyInputPacket.keyInput.up & 0x8001 ? "true" : "false",
			keyInputPacket.keyInput.down & 0x8001 ? "true" : "false",
			keyInputPacket.keyInput.right & 0x8001 ? "true" : "false",
			keyInputPacket.keyInput.left & 0x8001 ? "true" : "false",
			keyInputPacket.keyInput.action & 0x8001 ? "true" : "false");
		
		break;
	}
	case PACKET_TYPE_PLAYERS_DATA: {
		if (retval < sizeof(PlayersDataPacket)) {
			printf("Error: 패킷 사이즈 오류 PlayerDataPacket.\n");
			return retval;
		}

		PlayersDataPacket playerDataPacket;
		memcpy(&playerDataPacket, buf, sizeof(PlayersDataPacket));

		//player[clientID].SetPlayerData(playerDataPacket.player);
		break;
	}
	case PACKET_TYPE_CLIENT_DATA: {
		if (retval < sizeof(ClientDataPacket)) {
			printf("Error: 패킷 사이즈 오류 ClientDataPacket.\n");
			return retval;
		}

		ClientDataPacket clientDataPacket;
		memcpy(&clientDataPacket, buf, sizeof(ClientDataPacket));

		for (int i = 0; i < 5; i++)
			Player[clientID].Reflector.module[i] = clientDataPacket.module[i];
		Player[clientID].Reflector.RGB = clientDataPacket.color;

		printf("clientID: %d - clientColor: %d, clientModule: %d\n", 
			clientID, clientDataPacket.color, clientDataPacket.module[0]);
		break;
	}
	default:
		printf("error 패킷타입이 정의되지 않았습니다.\n");
		break;
	}

	return retval;
}

int SendS2CPacketAllPlayer(PacketType packetType)
{
	char buf[BUFSIZE + 1];
	int retval = 0;

	switch (packetType)
	{
	// InGame
	case PACKET_TYPE_IN_GAME: {
		GameDataPacket gameDataPacket;

		// GameDataPacket 채우기
		gameDataPacket.playerCount = clientCnt;
		for (int i = 0; i < clientCnt; i++)
			if (Player[i].Online)
				gameDataPacket.players[i] = Player[i];

		// 게임내 orb가 있을 때
		gameDataPacket.orbCount = 0;
		if (OrbHead->next != OrbHead) {
			Power_Orb* orb = OrbHead;
			int orbIdx = 0;
			while (orb->next != OrbHead)
			{
				orb = orb->next;
				gameDataPacket.orbs[orbIdx] = *orb;
				orbIdx++;
				if (orbIdx >= 10)
					break;
			}
			gameDataPacket.orbCount = orbIdx;
		}

		gameDataPacket.reactor = Reactor;

		/*
		for (int i = 0; i < clientCnt; i++)
		{
			printf("%d player - x: %f, y: %f\n", i, Player[i].Reflector.polar_x, Player[i].Reflector.polar_y);
		}
		*/

		for (int i = 0; i < clientCnt; i++)
			if (Player[i].Online)
				send(gClientSock[i], reinterpret_cast<char*>(&gameDataPacket), 
					sizeof(GameDataPacket), 0);

		return retval;
	}

	// GameStatusChange
	case PACKET_TYPE_CHANGE_GAME_STATE: {
		ChangeGameStatePacket gameStatePacket;

		gameStatePacket.gameState = gameState;
	
#ifdef DEBUG
		printf("gameStateChange - %s\n", 
			gameState == GAME_STATE_LOBBY ? "GameLobby" : 
			gameState == GAME_STATE_READY ? "GameReady" :
			gameState == GAME_STATE_GAME ? "InGame" :
			gameState == GAME_STATE_END ? "GameEnd" : "Defalt");
#endif // DEBUG

		for (int i = 0; i < clientCnt; i++)
			if (Player[i].Online)
				send(gClientSock[i], reinterpret_cast<char*>(&gameStatePacket),
					sizeof(ChangeGameStatePacket), 0);

		return retval;
	}

	case PACKET_TYPE_LOBBY: {
		LobbyDataPacket lobbyDataPacket;

		lobbyDataPacket.playerCount = clientCnt;
		for (int i = 0; i < clientCnt; i++)
			lobbyDataPacket.players[i] = Player[i];

		for (int i = 0; i < clientCnt; i++)
			if (Player[i].Online) {
				// clientID == player idx
				lobbyDataPacket.clientID = i;
				send(gClientSock[i], reinterpret_cast<char*>(&lobbyDataPacket),
					sizeof(LobbyDataPacket), 0);
			}

		return retval;
	}

	default:
		return 0;
	}
}

int SendS2CPacket(SOCKET clientSock, int clientID)
{
	char buf[BUFSIZE + 1];
	int retval = 0;

	switch (gameState)
	{
	// InGame
	case GAME_STATE_READY:
	case GAME_STATE_GAME: {
		GameDataPacket gameDataPacket;

		// GameDataPacket 채우기
		gameDataPacket.playerCount = clientCnt;
		for (int i = 0; i < clientCnt; i++)
			if (Player[i].Online) {
				//gameDataPacket.players[i].Reflector.polar_x = Player[i].Reflector.polar_x;
				//gameDataPacket.players[i].Reflector.polar_y = Player[i].Reflector.polar_y;
				gameDataPacket.players[i] = Player[i];
			}
		// gameDataPacket.ballData = ballData;

		/*
		for (int i = 0; i < clientCnt; i++)
		{
			printf("%d player - x: %f, y: %f\n", i, Player[i].Reflector.polar_x, Player[i].Reflector.polar_y);
		}
		*/
		// 데이터 전송
		send(clientSock, reinterpret_cast<char*>(&gameDataPacket), sizeof(GameDataPacket), 0);

		return retval;
		break;
	}

		// InLobby
	case GAME_STATE_LOBBY: {
		LobbyDataPacket lobbyDataPacket;

		// GameDataPacket 채우기
		lobbyDataPacket.playerCount = clientCnt;
		lobbyDataPacket.clientID = clientID;

		//lobbyDataPacket.players = new Player[clientCnt];
		//memcpy(lobbyDataPacket.players, players, sizeof(Player) * clientCnt);

		// 데이터 전송
		send(clientSock, reinterpret_cast<char*>(&lobbyDataPacket), sizeof(LobbyDataPacket), 0);

		//delete[] lobbyDataPacket.players;
		return retval; 
		}

	default:
		return 0;
	}
}