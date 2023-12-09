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

SOCKET GameClient::Sock()
{
	return mSock;
}

WSADATA GameClient::Wsa()
{
	return mWsa;
}

int GameClient::Init()
{
	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &mWsa) != 0)
		err_quit("WSAstartup()");

	// ���� ����
	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if (mSock == INVALID_SOCKET) err_quit("socket()");

	return 0;
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

		keyInputPacket.keyInput.up = player.Control[1];
		keyInputPacket.keyInput.down = player.Control[2];
		keyInputPacket.keyInput.right = player.Control[3];
		keyInputPacket.keyInput.left = player.Control[4];
		keyInputPacket.keyInput.action = player.Control[0];

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

void GameClient::SetClientID(int id)
{
	mClientID = id;
}

int GameClient::GetClientID()
{
	return mClientID;
}