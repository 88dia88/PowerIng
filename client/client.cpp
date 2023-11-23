#include "Common.h"
#include "Packit.h"
#include "Player.h"


char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    1024


int SendC2SPacket(SOCKET sock);
int RecvS2CPacket(SOCKET sock);


int gameState;
HANDLE hRecvEvent, hRenderEvent;
int clientID;
Player players[3];
int clientCnt;


int main(int argc, char* argv[])
{
	int retval;

	// ����� �μ��� ������ IP �ּҷ� ���
	if (argc > 1) SERVERIP = argv[1];

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	KeyInputPacket keyInputPacket;
	keyInputPacket.keyInput.up = true;
	keyInputPacket.keyInput.right = true;
	keyInputPacket.keyInput.action = true;

	// ������ ������(���� ����)
	retval = send(sock, reinterpret_cast<char*>(&keyInputPacket), sizeof(KeyInputPacket), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

int SendC2SPacket(SOCKET sock, const int PacketType)
{

}

int RecvS2CPacket(SOCKET sock)
{
	char buf[BUFSIZE + 1];
	int retval = recv(sock, buf, BUFSIZE, 0);

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