#include "Common.h"
#include "Globals.h"
#include "Packit.h"
#include "Player.h"
#include "GameTimer.h"
#include "PowerIng-main/Power_ing.h"
#include "PowerIng-main/Power_Math.h"
#include "PowerIng-main/resource.h"

#define SERVERPORT 9000
#define BUFSIZE 512

HANDLE hLobbyThread, hProcessThread;
DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI LobbyThread(LPVOID lpParam);
DWORD WINAPI ProcessThread(LPVOID lpParam);

HANDLE hProcessEvent[MAX_NUM_CLIENTS];         // process 쓰레드
HANDLE hClientKeyInputEvent[MAX_NUM_CLIENTS];  // 키 입력 이벤트
HANDLE hLobbyEvent[MAX_NUM_CLIENTS];           // 로비 이벤트

int gameState;
GameTimer gameTimer;
LONG clientCnt = 0;
CRITICAL_SECTION cs;
Player players[MAX_NUM_CLIENTS];

int GenerateClientIndex()
{
    int userID = -1;

    EnterCriticalSection(&cs);
    for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
        // 비활성화된 플레이어에 새로운 클라이언트를 할당한다.
        if (!players[i].GetState()) {
            userID = i;
            // 초기화
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

int main(int argc, char* argv[])
{
    int retval;

    // ProcessThread는 대기상태로 생성 LobbyThread가 종료할 때 스레드를 깨움
    hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
    hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

    // 이벤트 생성
    for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
        hProcessEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hLobbyEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET)
        err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR)
        err_quit("listen()");

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
        if (hThread == NULL)
            closesocket(client_sock);
        else {
            CloseHandle(hThread);
            InterlockedIncrement(&clientCnt); // 클라이언트 수 증가
        }
    }

    // 이벤트 종료
    for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
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

        players[clientID].SetKeyInput(keyInputPacket.keyInput);
        /*
        printf("키 인풋 처리, up=%s, down=%s, right=%s, left=%s, action=%s",
            keyInputPacket.keyInput.up ? "true" : "false",
            keyInputPacket.keyInput.down ? "true" : "false",
            keyInputPacket.keyInput.right ? "true" : "false",
            keyInputPacket.keyInput.left ? "true" : "false",
            keyInputPacket.keyInput.action ? "true" : "false");
        */
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

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
    gameTimer.Reset();

    while (gameState == GAME_STATE_LOBBY) {
        gameTimer.Tick();

    }
    // 프로세스 스레드 시작
    ResumeThread(hProcessThread);
    return 0;
}

int SendS2CPacket(SOCKET clientSock)
{
    char buf[BUFSIZE + 1];
    int retval = 0;

    switch (gameState)
    {
        // InGame
    case GAME_STATE_GAME:
        GameDataPacket gameDataPacket;

        // GameDataPacket 채우기
        gameDataPacket.playerCount = clientCnt;
        gameDataPacket.players = new Player[clientCnt];
        memcpy(gameDataPacket.players, players, sizeof(Player) * clientCnt);
        // gameDataPacket.ballData = ballData;

        // 데이터 전송
        send(clientSock, reinterpret_cast<char*>(&gameDataPacket), sizeof(GameDataPacket), 0);

        delete[] gameDataPacket.players;
        return retval;
        break;

        // InLobby
    case GAME_STATE_LOBBY:
        LobbyDataPacket lobbyDataPacket;

        // GameDataPacket 채우기
        lobbyDataPacket.playerCount = clientCnt;
        lobbyDataPacket.players = new Player[clientCnt];
        memcpy(lobbyDataPacket.players, players, sizeof(Player) * clientCnt);

        // 데이터 전송
        send(clientSock, reinterpret_cast<char*>(&lobbyDataPacket), sizeof(LobbyDataPacket), 0);

        delete[] lobbyDataPacket.players;
        return retval;
        break;

        // ReadyRound
    case GAME_STATE_READY:

        return retval;
        break;
    default:
        break;
    }
}

DWORD WINAPI ProcessThread(LPVOID lpParam)
{
    gameTimer.Reset();
    return 0;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ClientThread(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen;
    char buf[BUFSIZE + 1];

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    // 클라이언트 ID 생성 (0~2사이의 값으로 clients의 인덱스로 사용)
    int clientID = GenerateClientIndex();

    // SetEvent(ClientFlag[clientID]);  // 클라이언트가 준비되었음을 신호로 알림

    while (1) {
        // 데이터 받기
        retval = RecvC2SPacket(client_sock, clientID);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 데이터 처리
        buf[retval] = '\0';

        if (retval == PACKET_TYPE_KEY_INPUT) {
            // 클라이언트 입력완료        
            SetEvent(hClientKeyInputEvent[clientID]);
        }

        if (retval == PACKET_TYPE_LOBBY)
            SetEvent(hLobbyEvent[clientID]);

        // 데이터 처리 대기
        DWORD result = WaitForSingleObject(hProcessEvent[clientID], INFINITE);

        // 데이터 전송
        retval = SendS2CPacket(client_sock);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        else if (retval == 0)
            break;
    }

    // 소켓 닫기
    closesocket(client_sock);
    // 클라이언트 수 감소
    InterlockedDecrement(&clientCnt);
    // 플레이어 비활성화
    players[clientID].DisablePlayer();
    printf("[TCP 서버] 클라이언트 종료: ID=%d, IP 주소=%s, 포트 번호=%d\n",
        clientID, addr, ntohs(clientaddr.sin_port));
    return 0;
}
