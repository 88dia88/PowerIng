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

	// 명령행 인수가 있으면 IP 주소로 사용
	if (argc > 1) SERVERIP = argv[1];

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	// 데이터 통신에 사용할 변수
	KeyInputPacket keyInputPacket;
	keyInputPacket.keyInput.up = true;
	keyInputPacket.keyInput.right = true;
	keyInputPacket.keyInput.action = true;

	// 데이터 보내기(고정 길이)
	retval = send(sock, reinterpret_cast<char*>(&keyInputPacket), sizeof(KeyInputPacket), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
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

		players[clientID].SetKeyInput(keyInputPacket.keyInput);
		break;
	}
	case PACKET_TYPE_PLAYER_DATA: {
		if (retval < sizeof(PlayerDataPacket)) {
			printf("Error: 패킷 사이즈 오류 PlayerDataPacket.\n");
			return retval;
		}

		PlayerDataPacket playerDataPacket;
		memcpy(&playerDataPacket, buf, sizeof(PlayerDataPacket));

		players[clientID].SetPlayerData(playerDataPacket.player);
		break;
	}
	default:
		printf("error 패킷타입이 정의되지 않았습니다.\n");
		break;
	}

	return retval;
}