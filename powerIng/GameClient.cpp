#include "GameClient.h"
#include "Power_ing.h"


GameClient::GameClient()
{
	mOnline = false;
	mClientID = -1;
}

GameClient::~GameClient()
{
}

int GameClient::Init()
{
	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &mWsa) != 0)
		err_quit("WSAstartup()");

	// 소켓 생성
	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if (mSock == INVALID_SOCKET) err_quit("socket()");

	return 0;
}

void GameClient::ReleseSock()
{
	// 소켓 닫기
	closesocket(mSock);

	// 윈속 종료
	WSACleanup();
}

bool GameClient::Connect()
{
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, mServerIP.c_str(), &serveraddr.sin_addr);
	serveraddr.sin_port = htons(mServerPort);
	int retval = connect(mSock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
		return false;
	}

	mOnline = true;
	return true;
}

void GameClient::SetServer(const string& IP, const int port)
{
	mServerIP = IP;
	mServerPort = port;
}

bool GameClient::IsOnline()
{
	return mOnline;
}

int GameClient::SendPacket(const PacketType packetType, const Power_Player player)
{
	int retval = 0;

	switch (packetType)
	{
	case PACKET_TYPE_KEY_INPUT: {
		KeyInputPacket keyInputPacket;

		keyInputPacket.keyInput.up = player.upKeyDown;
		keyInputPacket.keyInput.down = player.downKeyDown;
		keyInputPacket.keyInput.right = player.rightKeyDown;
		keyInputPacket.keyInput.left = player.leftKeyDown;
		keyInputPacket.keyInput.action = player.actionKeyDown;

		retval = send(mSock, reinterpret_cast<char*>(&keyInputPacket), sizeof(KeyInputPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		break;
	}
	case PACKET_TYPE_CLIENT_DATA: {	// first send packet
		ClientDataPacket clientDataPacket;

		clientDataPacket.color = player.RGB;
		for (int i = 0; i < 5; i++)
			clientDataPacket.module[i] = player.Reflector.module[i];

		retval = send(mSock, reinterpret_cast<char*>(&clientDataPacket), sizeof(ClientDataPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		break;
	}
	default:
		break;
	}


	return 0;
}

int GameClient::RecvPacket(Power_Player& player)
{
	char buf[BUFSIZE + 1];
	int retval = recv(mSock, buf, BUFSIZE, 0);

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
		/*
	case PACKET_TYPE_PLAYERS_DATA: {
		if (retval < sizeof(PlayerDataPacket)) {
			printf("Error: 패킷 사이즈 오류 PlayerDataPacket.\n");
			return retval;
		}

		PlayersDataPacket playersDataPacket;
		memcpy(&playersDataPacket, buf, sizeof(PlayersDataPacket));

		players[clientID].SetPlayerData(playersDataPacket.player);
		break;
	}*/
	// 클라이언트 생성 초기 클라이언트가 부여받는 아이디 전달
	case PACKET_TYPE_LOBBY: {
		if (retval < sizeof(LobbyDataPacket)) {
			printf("Error: 패킷 사이즈 오류 LobbyDataPacket.\n");
			return retval;
		}
		LobbyDataPacket lobbyDataPacket;
		memcpy(&lobbyDataPacket, buf, sizeof(LobbyDataPacket));

		mClientID = lobbyDataPacket.clientID;
		gClientID = lobbyDataPacket.clientID;

		printf("ClientID: %d\n", mClientID);
		// 다른 플레이어 정보
		if (lobbyDataPacket.playerCount > 1) {
			//처리
		}
		break;
	}

	// 인게임 데이터 전달
	case PACKET_TYPE_IN_GAME: {
		if (retval < sizeof(GameDataPacket)) {
			printf("Error: 패킷 사이즈 오류 GameDataPacket.\n");
			return retval;
		}
		GameDataPacket gameDataPacket;
		memcpy(&gameDataPacket, buf, sizeof(GameDataPacket));

		int playerCnt = gameDataPacket.playerCount;
		for (int i = 0; i < playerCnt; i++)
		{
			if (Player[i].Online) {
				Player[i].Reflector.polar_x = gameDataPacket.players[i].Reflector.polar_x;
				Player[i].Reflector.polar_y = gameDataPacket.players[i].Reflector.polar_y;
			}
		}
		break;
	}

	default:
		printf("error 패킷타입이 정의되지 않았습니다.\n");
		break;
	}

	return retval;
}

void GameClient::SetClientID(int id)
{
	mClientID = id;
}

int GameClient::GetClientID()
{
	return mClientID;
}
