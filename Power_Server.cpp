#include "Common.h"
#include "Globals.h"
#include "Packit.h"
#include "Player.h"
#include "PowerIng-main/Power_ing.h"
#include "PowerIng-main/Power_Math.h"
#include "PowerIng-main/resource.h"

#define SERVERPORT 9000
#define BUFSIZE 512

void ReadServer();
DWORD WINAPI LobbyThread(LPVOID arg);
DWORD WINAPI ProcessThread(LPVOID arg);
DWORD WINAPI ClientThread(LPVOID arg);

HANDLE LobbyFlag[MAX_CLIENTS];    // Ŭ���̾�Ʈ���� �κ�� ���� �÷���
HANDLE StartFlag[MAX_CLIENTS];    // �κ񿡼� Ŭ���̾�Ʈ�� ���� �÷���
HANDLE ClientFlag[MAX_CLIENTS];   // Ŭ���̾�Ʈ ������ ���� �÷���
HANDLE ClientThreads[MAX_CLIENTS];  // Ŭ���̾�Ʈ ������ �ڵ� �迭

LONG clientCnt = 0;
CRITICAL_SECTION cs;
Player players[MAX_CLIENTS];
HANDLE ExitFlag[MAX_CLIENTS];
bool CheckClient[MAX_CLIENTS]{ false };
bool GameStart = false;

int GenerateClientIndex()
{
    int userID = -1;

    EnterCriticalSection(&cs);
    for (int i = 0; i < MAX_CLIENTS; i++)
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

int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // ���� ����
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed with error: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        fprintf(stderr, "listen() failed with error: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            fprintf(stderr, "accept() failed with error: %d\n", WSAGetLastError());
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
            fprintf(stderr, "CreateThread() failed with error: %d\n", GetLastError());
            closesocket(client_sock);
        }
        else {
            ClientThreads[clientCnt] = hThread;
            printf("����\n");
            InterlockedIncrement(&clientCnt); // Ŭ���̾�Ʈ �� ����
            CloseHandle(hThread);
        }
    }

    DeleteCriticalSection(&cs);
    closesocket(listen_sock);
    WSACleanup();

    return 0;
}

void ReadServer()
{
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    printf("Make Read Server \n");
    // ������ ����
    HANDLE lThread;

    lThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

    if (lThread != NULL) {
        CloseHandle(lThread); // ������ �ڵ��� �ݴ� ���� ����
    }
}

DWORD WINAPI LobbyThread(LPVOID arg)
{
    while (1) {
        WaitForMultipleObjects(MAX_CLIENTS, LobbyFlag, TRUE, INFINITE);

        printf("Create Lobby \n");

		if (!CheckClient[0] && !CheckClient[1] && !CheckClient[2]) {
            printf("Game Start!\n");
            for (int i = 0; i < clientCnt; ++i) {
                SetEvent(StartFlag[i]);
                ResetEvent(LobbyFlag[i]);  // ����: ��� ���� �̺�Ʈ ����
                ResetEvent(ClientFlag[i]);
            }
            GameStart = true;
            break;
        }
    }
    return 0;
}

DWORD WINAPI ProcessThread(LPVOID arg)
{
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

    InitializeCriticalSection(&cs);

    // Ŭ���̾�Ʈ ���� ���
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    // Ŭ���̾�Ʈ ID ���� (0~2������ ������ clients�� �ε����� ���)
    int clientID = GenerateClientIndex();
    std::cout << "Ŭ��" << clientID << std::endl;

    // �̺�Ʈ ����
    ClientFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    LobbyFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    ExitFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    StartFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);

    SetEvent(ClientFlag[clientID]);  // Ŭ���̾�Ʈ�� �غ�Ǿ����� ��ȣ�� �˸�

    while (1) {
        // ������ �ޱ�
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            int error = WSAGetLastError();
            printf("[TCP ����] recv() �Լ����� ���� �߻�. ���� �ڵ�: %d\n", error);
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // ������ ó��
        buf[retval] = '\0';

        // ������ ������
        retval = send(client_sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }

        // ProcessPacket(buf, retval);
        printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
        if (strcmp(buf, "Ready") == 0) {
            printf("LobbyFlag %d ON! \n", clientID);
            CheckClient[clientID] = true;
            SetEvent(LobbyFlag[clientID]);
            //WaitForSingleObject(ClientFlag[clientID], INFINITE);
            if (!players[clientID].IsReady()) {
                printf("Go to Lobby! \n");
                ReadServer();
            }
        }

        else if (strcmp(buf, "Set") == 0) {
            players[clientID].SetPlayer(clientID);
            players[clientID].SetReady(true);
            CheckClient[clientID] = false;
            SetEvent(LobbyFlag[clientID]);
            SetEvent(StartFlag[clientID]);
            ReadServer();
        }

        // ������ ó���� �Ϸ�Ǿ����� ��ȣ�� �˸�
        SetEvent(ClientFlag[clientID]);          
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
