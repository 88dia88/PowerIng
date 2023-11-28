#include "GameClient.h"

GameClient::GameClient()
{
}

GameClient::~GameClient()
{
}

int GameClient::Init()
{
    // ���� �ʱ�ȭ
    mWsa;
    if (WSAStartup(MAKEWORD(2, 2), &mWsa) != 0)
        return 1;

    // ���� ����
    mSock = socket(AF_INET, SOCK_STREAM, 0);
    if (mSock == INVALID_SOCKET) err_quit("socket()");
}

void GameClient::ReleseSock()
{
	// ���� �ݱ�
	closesocket(mSock);

	// ���� ����
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
    if (retval == SOCKET_ERROR) err_quit("connect()");
}

void GameClient::SetServer(const string& IP, const int port)
{
    mServerIP = IP;
    mServerPort = port;
}

int GameClient::SendPacket(const PacketType packetType)
{
	int retval = 0;

	switch (packetType)
	{
	case PACKET_TYPE_KEY_INPUT:
		KeyInputPacket keyInputPacket;

		int retval = send(mSock, reinterpret_cast<char*>(&keyInputPacket), sizeof(KeyInputPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		break;
	default:
		break;
	}


    return 0;
}

int GameClient::RecvPacket()
{
	char buf[BUFSIZE + 1];
	int retval = recv(mSock, buf, BUFSIZE, 0);

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
		break;
	}
	case PACKET_TYPE_PLAYER_DATA: {
		if (retval < sizeof(PlayerDataPacket)) {
			printf("Error: ��Ŷ ������ ���� PlayerDataPacket.\n");
			return retval;
		}

		PlayerDataPacket playerDataPacket;
		memcpy(&playerDataPacket, buf, sizeof(PlayerDataPacket));

		players[clientID].SetPlayerData(playerDataPacket.player);
		break;
	}
	default:
		printf("error ��ŶŸ���� ���ǵ��� �ʾҽ��ϴ�.\n");
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
