// #include "Power_ing.h"
#include "Common.h"
#include "Globals.h"
#include "Packit.h"
#include "Player.h"
#include "GameTimer.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define WM_SOCKET  (WM_USER+1)

HANDLE hLobbyThread, hProcessThread;
DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI LobbyThread(LPVOID lpParam);
DWORD WINAPI ProcessThread(LPVOID lpParam);

HANDLE hProcessEvent[MAX_NUM_CLIENTS];         // process ������
HANDLE hClientKeyInputEvent[MAX_NUM_CLIENTS];  // Ű �Է� �̺�Ʈ
HANDLE hLobbyEvent[MAX_NUM_CLIENTS];           // �κ� �̺�Ʈ

int gameState;
bool allReady = true;
GameTimer gameTimer;
LONG clientCnt = 0;
CRITICAL_SECTION cs;
Player players[MAX_NUM_CLIENTS];

// ���� ���� ������ ���� ����ü�� ����
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	bool recvdelayed;
	SOCKETINFO* next;
};
SOCKETINFO* SocketInfoList;

void ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
// ���� ���� ���� �Լ�
bool AddSocketInfo(SOCKET sock);
SOCKETINFO* GetSocketInfo(SOCKET sock);
void RemoveSocketInfo(SOCKET sock);
int GenerateClientIndex();
int RecvC2SPacket(SOCKET clientSock, int clientID);
int SendS2CPacket(SOCKET clientSock);


int main(int argc, char* argv[])
{
	int retval;

	// ������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("MyWndClass");
	if (!RegisterClass(&wndclass)) return 1;

	// ������ ����
	HWND hWnd = CreateWindow(_T("MyWndClass"), _T("TCP ����"),
		WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, NULL, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ProcessThread�� �����·� ���� LobbyThread�� ������ �� �����带 ����
	hProcessThread = CreateThread(NULL, 0, ProcessThread, NULL, CREATE_SUSPENDED, NULL);
	hLobbyThread = CreateThread(NULL, 0, LobbyThread, NULL, 0, NULL);

	// �̺�Ʈ ����
	for (int i = 0; i < MAX_NUM_CLIENTS; i++) {
		hProcessEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hClientKeyInputEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hLobbyEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	// ���� ����
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

	// WSAAsyncSelect()
	retval = WSAAsyncSelect(listen_sock, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	// �޽��� ����
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ���� ����
	WSACleanup();
	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
//	//--------------------------------------------------------------------------------------------------------------//
//	srand((int)time(NULL));
//	PAINTSTRUCT ps;
//
//	POINTS Mouse = MAKEPOINTS(lParam);
//
//	static bool Ingame = true;
//	// �߰�����-�޴�-------
//	static int SelectedButton = 0; // �� ��ư���� ���콺�� �ö� ������ ���� �����
//	static int Menu_Type = 0; // �޴� ����
//	/*
//	0 : ���θ޴�
//	1 : ���Ӹ��
//	2 : Ŀ���͸���¡
//	3 : �ɼ�
//	31 : ����
//	32 : ����
//	33 : �����
//	*/
//	// -------------------
//
//	// �߰�����-����-------
//	static int GameMode = 0;
//	/*
//	0 : �̱��÷���
//	1 : �������
//	2 : ���� ���� ���
//	11 : ���� ���
//	12 : ���� ���
//	*/
//	static int GameStatus = -1;
//	/*
//	0 : ESC
//	1 : �÷��� ����
//	2 : �غ� ����
//	-1 : ����/���� �ִϸ��̼�
//	-2 : ���� ȭ��
//	*/
//	static int EscMode = 0;
//	static bool DisplayGame = false;
//
//	static bool OrbLaunch = false;
//
//	static int CustomRGB[4] = { RGBTemplate_Yellow, 255, 255, 0 };
//
//	static struct Power_Player MainPlayer;
//
//
//	int MassSel = 1;
//	int rgb[9] = { RGBTemplate_Green,
//		RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green,
//		RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan };
//	// -------------------
//
//
//
//	//PlgBlt = ȸ���ϱ� ���� 3��(�»�,���,����)�� ��ǥ �ʿ�
//
//	if (iMsg == WM_CREATE) {
//		GameStart = false;
//		EffectHead->next = EffectHead;
//		OrbHead->next = OrbHead;
//
//		/*
//		ReflectorHead->next = ReflectorHead;
//		ReflectorHead = ReflectorReset(ReflectorHead);
//		*/
//
//		Setting = SettingReset(Setting);
//
//		MainPlayer = PlayerReset(MainPlayer, 0);
//
//		for (int i = 1; i < 7; i++)
//		{
//			Player[i] = PlayerReset(Player[i], i);
//			Player[i].Reflector.polar_x = i / 6.0;
//			Player[i].Online = false; // �ּ� ó���ϸ� �߰� �г��� Ȱ��ȭ��
//			//if (i % 2 == 0) Player[i].Online = false; // �ּ� ó���ϸ� 6���� �г��� Ȱ��ȭ��
//
//			/* //�Ʒ� �ڵ��� �ּ� ó���� �����ϸ� �÷��̾ 9������ ������ ������ �����
//			while (true) {
//				if (rand() % 9 == 1) Player[i].RGB = RGBTemplate_Red;
//				else if (rand() % 9 == 2) Player[i].RGB = RGBTemplate_Green;
//				else if (rand() % 9 == 3) Player[i].RGB = RGBTemplate_Blue;
//				else if (rand() % 9 == 4) Player[i].RGB = RGBTemplate_Yellow;
//				else if (rand() % 9 == 5) Player[i].RGB = RGBTemplate_Magenta;
//				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Cyan;
//				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Black;
//				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Gray;
//				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_White;
//
//				int ColorDupe = 0;
//				for (int j = 1; j < i; j++) if (Player[i].RGB == Player[j].RGB) ColorDupe++;
//				if (ColorDupe == 0) break;
//			}
//			*/
//			Player[i].Reflector.RGB = Player[i].RGB;
//		}
//
//		GeneralReset();
//		Temperture = Kelvin, Mole = MaxMole * 0.5;
//		PreTime = -25;
//		Orbcount = 3;
//		TotalScore = 0;
//		DisplayLoad();
//		DisplayOrbLoad();
//
//		DisplayColorApply(MainPlayer.RGB);
//		DisplayReflectorColorApply(MainPlayer.RGB);
//
//		DisplayPlayerColorApply(MainPlayer.RGB, 0);
//
//		for (int i = 1; i < 7; i++) DisplayPlayerColorApply(Player[i].RGB, i);
//
//		Control.Left = 0x25;
//		Control.Right = 0X27;
//		Control.Up = 0X26;
//		Control.Down = 0X28;
//
//		AnimationTime_Door = 100;
//		SetTimer(hWnd, 0, 1000 / 60, NULL);
//	}
//	if (Ingame) {
//		switch (iMsg) {
//		case WM_KEYDOWN:
//			if (GameStatus == -2) {
//				if (DisplayGame == false) {
//					Menu_Type = 0;
//					EscMode = 0;
//					Ingame = false;
//					TotalScore = 0;
//					GeneralReset();
//					break;
//				}
//			}
//			if (wParam == VK_ESCAPE) {
//				if (GameStatus == 1) {
//					EscMode = 101;
//					GameStatus = 0;
//				}
//				else if (GameStatus == 2) {
//					EscMode = 201;
//					GameStatus = 0;
//				}
//				else if (GameStatus == 0 and EscMode % 100 == 2) {
//					EscMode++;
//					DisplayGame = true;
//				}
//			}
//			else if (wParam == VK_RETURN) {
//				switch (GameStatus) {
//				case 1:
//					if (Reactor.cherenkov == false) {
//						if (Reactor.cherenkovmeter == 100) Reactor.cherenkov = true;
//						else if (Reactor.cherenkovmeter >= 875) {
//							//���� ���� ���� �ʾ����� �� ������ �ߵ�
//							Reactor.cherenkov = true;
//						}
//					}
//					break;
//				case 2:
//					if (PressureCheck())
//					{
//						OrbLaunch = true;
//						GameStatus = 1;
//					}
//					break;
//				}
//			}
//			break;
//		case WM_MOUSEMOVE:
//			switch (GameStatus) {
//			case 0:
//				if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
//				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
//				else if (UIButtonSelected(-700, 315, 750, 150, Mouse)) SelectedButton = 4;
//				else SelectedButton = 0;
//				break;
//			}
//			break;
//		case WM_LBUTTONDOWN:
//			switch (GameStatus) {
//			case -2:
//				if (DisplayGame == false) {
//					Menu_Type = 0;
//					EscMode = 0;
//					Ingame = false;
//					TotalScore = 0;
//					GeneralReset();
//					break;
//				}
//			case 0:
//				if (EscMode % 100 == 2) {
//					switch (SelectedButton) {
//					case 2:
//						EscMode++;
//						DisplayGame = true;
//						break;
//					case 3:
//
//						break;
//					case 4:
//						if (DisplayGame == false) {
//							Menu_Type = 0;
//							EscMode = 0;
//							Ingame = false;
//							TotalScore = 0;
//							GeneralReset();
//							break;
//						}
//					}
//					SelectedButton = 0;
//				}
//				break;
//			}
//			break;
//		case WM_SOCKET: // ���� ���� ������ �޽���
//			ProcessSocketMessage(hWnd, iMsg, wParam, lParam);
//			break;
//		case WM_TIMER: //GetAsyncKeyState - Ű�� ���� ���·� ����Ǵ��Լ� (�������)http://www.silverwolf.co.kr/cplusplus/4842
//			switch (wParam)
//			{
//			case 0:
//				switch (GameStatus) {
//				case -2:
//					if (DisplayGame) {
//						if (AnimationTime_Door < 100) AnimationTime_Door += 2;
//						else {
//							AnimationTime_Door = 100;
//							DisplayGame = false;
//						}
//					}
//				case -1:
//					if (DisplayGame and GameStatus != -2) {
//						if (AnimationTime_Door > 0) AnimationTime_Door -= 2;
//						else {
//							AnimationTime_Door = 0;
//							GameStatus = 2;
//						}
//					}
//				case 0:
//					if (EscMode != 0) {
//						if (DisplayGame) {
//							if (EscMode % 100 == 1) {
//								if (AnimationTime_Door < 100) AnimationTime_Door += 5;
//								else {
//									AnimationTime_Door = 100;
//									EscMode++;
//									DisplayGame = false;
//								}
//							}
//							else if (EscMode % 100 == 3) {
//								if (AnimationTime_Door > 0) AnimationTime_Door -= 5;
//								else {
//									AnimationTime_Door = 0;
//									GameStatus = int(EscMode / 100);
//									EscMode = 0;
//								}
//							}
//							break;
//						}
//					}
//				case 1: case 2:
//					switch (GameMode)
//					{
//					case 0:
//						Time_Server++;
//
//						if (Reactor.meltdown == false and Orbcount < 0) {
//							GameStatus = -2;
//							//���ӿ���
//						}
//						else
//						{
//							if (ReactorMeltdown() and GameStatus == 1) GameStatus = 2;
//							ReactorCherenkov();
//							if (GameStatus == 1) {
//								CollisionDetect(OrbHead);
//								if (OrbLaunch) {
//									OrbLaunch = false;
//									ColliderColor = 0;
//									OrbCreate(OrbHead, OrbType, true, 0, 0, 0.25);
//								}
//							}
//						}
//
//						for (int i = 0; i < 7; i++)
//						{
//							if (Player[i].Online) {
//								Player[i].Reflector = ReflectorProcess(Player[i].Reflector, (GameStatus == 1));
//							}
//						}
//
//						//�� �Ʒ��� Ŭ���̾�Ʈ ���̵�
//
//						if (Time_Server > Time + 30) Time = Time_Server;
//						else if (Time_Server > Time) Time += 2;
//						else Time++;
//
//						//Player[0].Reflector = ReflectorControl(Player[0].Reflector, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));
//
//						Player[0].Reflector = ReflectorPosition(Player[0].Reflector, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));
//
//						break;
//					case 1:
//						if (Reactor.meltdown == false and Orbcount < 0) {
//							Orbcount = 0;
//							GameStatus = -2;
//							//���ӿ���
//						}
//						else {
//							//����
//						}
//						break;
//					case 2:
//						if (Reactor.meltdown == false and Orbcount < 0) {
//							Orbcount = 0;
//							GameStatus = -2;
//							//���ӿ���
//						}
//						else {
//							//���� ����
//						}
//						break;
//					case 11:
//						//��Ÿ ����
//						break;
//					}
//					break;
//				}
//				break;
//			}
//			InvalidateRgn(hWnd, NULL, false);
//			break;
//		case WM_DESTROY:
//			PostQuitMessage(0);
//			break;
//		}
//	}
//	return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- ���� �� �޽��� ���� ������ �޽����� OS��
}


// ���� ���� �޽��� ó��
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ������ ��ſ� ����� ����
	int retval;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	SOCKETINFO* ptr;

	// ���� �߻� ���� Ȯ��
	if (WSAGETSELECTERROR(lParam)) {
		err_display(WSAGETSELECTERROR(lParam));
		RemoveSocketInfo(wParam);
		return;
	}

	// �޽��� ó��
	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_ACCEPT:
		addrlen = sizeof(clientaddr);
		client_sock = accept(wParam, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			return;
		}
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		AddSocketInfo(client_sock);
		retval = WSAAsyncSelect(client_sock, hWnd,
			WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
		if (retval == SOCKET_ERROR) {
			err_display("WSAAsyncSelect()");
			RemoveSocketInfo(client_sock);
		}
		break;
	case FD_READ:
		ptr = GetSocketInfo(wParam);
		if (ptr->recvbytes > 0) {
			ptr->recvdelayed = true;
			return;
		}
		// ������ �ޱ�
		retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			RemoveSocketInfo(wParam);
			return;
		}
		ptr->recvbytes = retval;
		// ���� ������ ���
		ptr->buf[retval] = '\0';
		addrlen = sizeof(clientaddr);
		getpeername(wParam, (struct sockaddr*)&clientaddr, &addrlen);
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), ptr->buf);
	case FD_WRITE:
		ptr = GetSocketInfo(wParam);
		if (ptr->recvbytes <= ptr->sendbytes)
			return;
		// ������ ������
		retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
			ptr->recvbytes - ptr->sendbytes, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			RemoveSocketInfo(wParam);
			return;
		}
		ptr->sendbytes += retval;
		// ���� �����͸� ��� ���´��� üũ
		if (ptr->recvbytes == ptr->sendbytes) {
			ptr->recvbytes = ptr->sendbytes = 0;
			if (ptr->recvdelayed) {
				ptr->recvdelayed = false;
				PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
			}
		}
		break;
	case FD_CLOSE:
		RemoveSocketInfo(wParam);
		break;
	}
}

// ���� ���� �߰�
bool AddSocketInfo(SOCKET sock)
{
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return false;
	}
	// ���� ���� ����Ʈ�� ���� �߰�
	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->recvdelayed = false;
	ptr->next = SocketInfoList;
	SocketInfoList = ptr;
	return true;
}

// ���� ���� ���
SOCKETINFO* GetSocketInfo(SOCKET sock)
{
	SOCKETINFO* ptr = SocketInfoList;
	while (ptr) {
		if (ptr->sock == sock)
			return ptr;
		ptr = ptr->next;
	}
	return NULL;
}

// ���� ���� ����
void RemoveSocketInfo(SOCKET sock)
{
	// Ŭ���̾�Ʈ ���� ���
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (struct sockaddr*)&clientaddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// ���� ���� ����Ʈ���� ���� ����
	SOCKETINFO* curr = SocketInfoList;
	SOCKETINFO* prev = NULL;
	while (curr) {
		if (curr->sock == sock) {
			if (prev)
				prev->next = curr->next;
			else
				SocketInfoList = curr->next;
			closesocket(curr->sock); // ���� �ݱ�
			delete curr;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

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

DWORD WINAPI LobbyThread(LPVOID lpParam)
{
	WaitForMultipleObjects(MAX_NUM_CLIENTS, hLobbyEvent, TRUE, INFINITE);

	gameTimer.Reset();

	while (1) {
		gameState = GAME_STATE_LOBBY;

		// �κ� ���� (���� �� ��� ����)

		gameTimer.Tick();
		for (int i = 0; i < MAX_NUM_CLIENTS; ++i) {
			if (!players[i].IsReady())
				allReady = false;
		}
		if (allReady) {
			gameState = GAME_STATE_READY;
			while (gameState == GAME_STATE_READY) {
				if (1) { // ���� ���� ��ư? ������
					allReady = false;
					gameState = GAME_STATE_GAME;
					break;
				}
				else if (2) { // ���� ���� �����Ѵٸ�
					allReady = false;
					gameState = GAME_STATE_LOBBY;
					break;
				}
			}
		}
		if (3) { // �κ� Ż��? ���� ����?
			gameState = GAME_STATE_END;
			break;
		}
	}

	ResetEvent(hLobbyEvent);
	// ���μ��� ������ ����
	ResumeThread(hProcessThread);
	return 0;
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

		// Ŭ���̾�Ʈ �Է¿Ϸ�        
		SetEvent(hClientKeyInputEvent[clientID]);

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

		break;
	}
	case PACKET_TYPE_LOBBY: {
		if (retval < sizeof(LobbyDataPacket)) {
			printf("Error: ��Ŷ ������ ���� LobbyDataPacket.\n");
			return retval;
		}

		LobbyDataPacket lobbyDataPacket;
		memcpy(&lobbyDataPacket, buf, sizeof(LobbyDataPacket));

		players[clientID].SetPlayerData(*lobbyDataPacket.players);
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

		// LobbyDataPacket ä���
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