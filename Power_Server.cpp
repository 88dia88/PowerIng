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

HANDLE hProcessEvent[MAX_NUM_CLIENTS];         // process ������
HANDLE hClientKeyInputEvent[MAX_NUM_CLIENTS];  // Ű �Է� �̺�Ʈ
HANDLE hLobbyEvent[MAX_NUM_CLIENTS];           // �κ� �̺�Ʈ

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

int main(int argc, char* argv[])
{
    int retval;

    // ProcessThread�� �����·� ���� LobbyThread�� ������ �� �����带 ����
    hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
    hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

    // �̺�Ʈ ����
    for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
        hProcessEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hLobbyEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
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
        if (hThread == NULL)
            closesocket(client_sock);
        else {
            CloseHandle(hThread);
            InterlockedIncrement(&clientCnt); // Ŭ���̾�Ʈ �� ����
        }
    }

    // �̺�Ʈ ����
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

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
    gameTimer.Reset();

    while (gameState == GAME_STATE_LOBBY) {
        gameTimer.Tick();

    }
    // ���μ��� ������ ����
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

        // GameDataPacket ä���
        gameDataPacket.playerCount = clientCnt;
        gameDataPacket.players = new Player[clientCnt];
        memcpy(gameDataPacket.players, players, sizeof(Player) * clientCnt);
        // gameDataPacket.ballData = ballData;

        // ������ ����
        send(clientSock, reinterpret_cast<char*>(&gameDataPacket), sizeof(GameDataPacket), 0);

        delete[] gameDataPacket.players;
        return retval;
        break;

        // InLobby
    case GAME_STATE_LOBBY:
        LobbyDataPacket lobbyDataPacket;

        // GameDataPacket ä���
        lobbyDataPacket.playerCount = clientCnt;
        lobbyDataPacket.players = new Player[clientCnt];
        memcpy(lobbyDataPacket.players, players, sizeof(Player) * clientCnt);

        // ������ ����
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

// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ClientThread(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen;
    char buf[BUFSIZE + 1];

    // Ŭ���̾�Ʈ ���� ���
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    // Ŭ���̾�Ʈ ID ���� (0~2������ ������ clients�� �ε����� ���)
    int clientID = GenerateClientIndex();

    // SetEvent(ClientFlag[clientID]);  // Ŭ���̾�Ʈ�� �غ�Ǿ����� ��ȣ�� �˸�

    while (1) {
        // ������ �ޱ�
        retval = RecvC2SPacket(client_sock, clientID);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // ������ ó��
        buf[retval] = '\0';

        if (retval == PACKET_TYPE_KEY_INPUT) {
            // Ŭ���̾�Ʈ �Է¿Ϸ�        
            SetEvent(hClientKeyInputEvent[clientID]);
        }

        if (retval == PACKET_TYPE_LOBBY)
            SetEvent(hLobbyEvent[clientID]);

        // ������ ó�� ���
        DWORD result = WaitForSingleObject(hProcessEvent[clientID], INFINITE);

        // ������ ����
        retval = SendS2CPacket(client_sock);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        else if (retval == 0)
            break;
    }

    // ���� �ݱ�
    closesocket(client_sock);
    // Ŭ���̾�Ʈ �� ����
    InterlockedDecrement(&clientCnt);
    // �÷��̾� ��Ȱ��ȭ
    players[clientID].DisablePlayer();
    printf("[TCP ����] Ŭ���̾�Ʈ ����: ID=%d, IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
        clientID, addr, ntohs(clientaddr.sin_port));
    return 0;
}
