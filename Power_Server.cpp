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

HANDLE LobbyFlag[MAX_CLIENTS];    // 클라이언트에서 로비로 연결 플레그
HANDLE StartFlag[MAX_CLIENTS];    // 로비에서 클라이언트로 연결 플레그
HANDLE ClientFlag[MAX_CLIENTS];   // 클라이언트 쓰레드 생성 플레그
HANDLE ClientThreads[MAX_CLIENTS];  // 클라이언트 스레드 핸들 배열

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

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // 소켓 생성
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

    // 데이터 통신에 사용할 변수
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

        // 접속한 클라이언트 정보 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            addr, ntohs(clientaddr.sin_port));

        // 스레드 생성
        hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            fprintf(stderr, "CreateThread() failed with error: %d\n", GetLastError());
            closesocket(client_sock);
        }
        else {
            ClientThreads[clientCnt] = hThread;
            printf("생성\n");
            InterlockedIncrement(&clientCnt); // 클라이언트 수 증가
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
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    printf("Make Read Server \n");
    // 쓰레드 생성
    HANDLE lThread;

    lThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

    if (lThread != NULL) {
        CloseHandle(lThread); // 쓰레드 핸들을 닫는 것이 좋음
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
                ResetEvent(LobbyFlag[i]);  // 수정: 대기 중인 이벤트 해제
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

// 클라이언트와 데이터 통신
DWORD WINAPI ClientThread(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen;
    char buf[BUFSIZE + 1];

    InitializeCriticalSection(&cs);

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    // 클라이언트 ID 생성 (0~2사이의 값으로 clients의 인덱스로 사용)
    int clientID = GenerateClientIndex();
    std::cout << "클라" << clientID << std::endl;

    // 이벤트 생성
    ClientFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    LobbyFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    ExitFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);
    StartFlag[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);

    SetEvent(ClientFlag[clientID]);  // 클라이언트가 준비되었음을 신호로 알림

    while (1) {
        // 데이터 받기
        retval = recv(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            int error = WSAGetLastError();
            printf("[TCP 서버] recv() 함수에서 오류 발생. 오류 코드: %d\n", error);
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 데이터 처리
        buf[retval] = '\0';

        // 데이터 보내기
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

        // 데이터 처리가 완료되었음을 신호로 알림
        SetEvent(ClientFlag[clientID]);          
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
