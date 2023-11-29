#include "Power_ing.h"
#include "Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define WM_SOCKET  (WM_USER+1)

// 소켓 정보 저장을 위한 구조체와 변수
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
// 소켓 정보 관리 함수
bool AddSocketInfo(SOCKET sock);
SOCKETINFO* GetSocketInfo(SOCKET sock);
void RemoveSocketInfo(SOCKET sock);

int main(int argc, char* argv[])
{
	int retval;

	// 윈도우 클래스 등록
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

	// 윈도우 생성
	HWND hWnd = CreateWindow(_T("MyWndClass"), _T("TCP 서버"),
		WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, NULL, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	// 메시지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 윈속 종료
	WSACleanup();
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//--------------------------------------------------------------------------------------------------------------//
	srand((int)time(NULL));
	PAINTSTRUCT ps;

	POINTS Mouse = MAKEPOINTS(lParam);

	static bool Ingame = true;
	// 추가변수-메뉴-------
	static int SelectedButton = 0; // 각 버튼위에 마우스가 올라가 있으면 값이 변경됨
	static int Menu_Type = 0; // 메뉴 종류
	/*
	0 : 메인메뉴
	1 : 게임모드
	2 : 커스터마이징
	3 : 옵션
	31 : 게임
	32 : 비디오
	33 : 오디오
	*/
	// -------------------
	
	// 추가변수-게임-------
	static int GameMode = 0;
	/*
	0 : 싱글플레이
	1 : 협동모드
	2 : 점수 경쟁 모드
	11 : 경쟁 모드
	12 : 진영 모드
	*/
	static int GameStatus = -1;
	/*
	0 : ESC
	1 : 플레이 상태
	2 : 준비 상태
	-1 : 시작/종료 애니메이션
	-2 : 종료 화면
	*/
	static int EscMode = 0;
	static bool DisplayGame = false;

	static bool OrbLaunch = false;

	static struct Power_Player MainPlayer;

	int MassSel = 1;
	// -------------------



	//PlgBlt = 회전하기 위한 3개(좌상,우상,좌하)의 좌표 필요

	if (iMsg == WM_CREATE) {
		GameStart = false;
		OrbHead->next = OrbHead;

		/*
		ReflectorHead->next = ReflectorHead;
		ReflectorHead = ReflectorReset(ReflectorHead);
		*/

		Setting = SettingReset(Setting);

		MainPlayer = PlayerReset(MainPlayer, 0);
		
		for (int i = 1; i < 7; i++)
		{
			Player[i] = PlayerReset(Player[i], i);
			Player[i].Reflector.polar_x = i / 6.0;
			Player[i].Online = false; // 주석 처리하면 추가 패널이 활성화됨
			//if (i % 2 == 0) Player[i].Online = false; // 주석 처리하면 6개의 패널이 활성화됨

			/* //아래 코드의 주석 처리를 해제하면 플레이어에 9가지중 무작위 색상이 적용됨
			while (true) {
				if (rand() % 9 == 1) Player[i].RGB = RGBTemplate_Red;
				else if (rand() % 9 == 2) Player[i].RGB = RGBTemplate_Green;
				else if (rand() % 9 == 3) Player[i].RGB = RGBTemplate_Blue;
				else if (rand() % 9 == 4) Player[i].RGB = RGBTemplate_Yellow;
				else if (rand() % 9 == 5) Player[i].RGB = RGBTemplate_Magenta;
				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Cyan;
				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Black;
				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_Gray;
				else if (rand() % 9 == 6) Player[i].RGB = RGBTemplate_White;

				int ColorDupe = 0;
				for (int j = 1; j < i; j++) if (Player[i].RGB == Player[j].RGB) ColorDupe++;
				if (ColorDupe == 0) break;
			}
			*/
			Player[i].Reflector.RGB = Player[i].RGB;
		}

		GeneralReset();
		Temperture = Kelvin, Mole = MaxMole * 0.5;
		PreTime = -25;
		Orbcount = 3;
		TotalScore = 0;

		Control.Left = 0x25;
		Control.Right = 0X27;
		Control.Up = 0X26;
		Control.Down = 0X28;

		AnimationTime_Door = 100;
		SetTimer(hWnd, 0, 1000 / 60, NULL);
	}
	if (Ingame) {
		switch (iMsg) {
		case WM_KEYDOWN:
			if (GameStatus == -2) {
				if (DisplayGame == false) {
					Menu_Type = 0;
					EscMode = 0;
					Ingame = false;
					TotalScore = 0;
					GeneralReset();
					break;
				}
			}
			if (wParam == VK_ESCAPE) {
				if (GameStatus == 1) {
					EscMode = 101;
					GameStatus = 0;
				}
				else if (GameStatus == 2) {
					EscMode = 201;
					GameStatus = 0;
				}
				else if (GameStatus == 0 and EscMode % 100 == 2) {
					EscMode ++;
					DisplayGame = true;
				}
			}
			else if (wParam == VK_RETURN) {
				switch (GameStatus) {
				case 1:
					if (Reactor.cherenkov == false) {
						if (Reactor.cherenkovmeter == 100) Reactor.cherenkov = true;
						else if (Reactor.cherenkovmeter >= 875) {
							//완전 충전 되지 않았으면 꾹 눌러서 발동
							Reactor.cherenkov = true;
						}
					}
					break;
				case 2:
					if (PressureCheck())
					{
						OrbLaunch = true;
						GameStatus = 1;
					}
					break;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			switch (GameStatus) {
			case -2:
				if (DisplayGame == false) {
					Menu_Type = 0;
					EscMode = 0;	
					Ingame = false;
					TotalScore = 0;
					GeneralReset();
					break;
				}
			case 0:
				if (EscMode % 100 == 2) {
					switch (SelectedButton) {
					case 2:
						EscMode++;
						DisplayGame = true;
						break;
					case 3:

						break;
					case 4:
						if (DisplayGame == false) {
							Menu_Type = 0;
							EscMode = 0;
							Ingame = false;
							TotalScore = 0;
							GeneralReset();
							break;
						}
					}
					SelectedButton = 0;
				}
				break;
			}
			break;
		case WM_SOCKET: // 소켓 관련 윈도우 메시지
			ProcessSocketMessage(hWnd, iMsg, wParam, lParam);
			break;
		case WM_TIMER: //GetAsyncKeyState - 키가 눌린 상태로 진행되는함수 (끊김없이)http://www.silverwolf.co.kr/cplusplus/4842
			switch (wParam)
			{
			case 0:
				switch (GameStatus) {
				case -2:
					if (DisplayGame) {
						if (AnimationTime_Door < 100) AnimationTime_Door += 2;
						else {
							AnimationTime_Door = 100;
							DisplayGame = false;
						}
					}
				case -1:
					if (DisplayGame and GameStatus != -2) {
						if (AnimationTime_Door > 0) AnimationTime_Door -= 2;
						else {
							AnimationTime_Door = 0;
							GameStatus = 2;
						}
					}
				case 0:
					if (EscMode != 0) {
						if (DisplayGame) {
							if (EscMode % 100 == 1) {
								if (AnimationTime_Door < 100) AnimationTime_Door += 5;
								else {
									AnimationTime_Door = 100;
									EscMode++;
									DisplayGame = false;
								}
							}
							else if (EscMode % 100 == 3) {
								if (AnimationTime_Door > 0) AnimationTime_Door -= 5;
								else {
									AnimationTime_Door = 0;
									GameStatus = int(EscMode / 100);
									EscMode = 0;
								}
							}
							break;
						}
					}
				case 1: case 2:
					switch (GameMode)
					{
					case 0:
						Time_Server++;

						if (Reactor.meltdown == false and Orbcount < 0) {
							GameStatus = -2;
							//게임오버
						}
						else
						{
							if (ReactorMeltdown() and GameStatus == 1) GameStatus = 2;
							ReactorCherenkov();
							if (GameStatus == 1) {
								CollisionDetect(OrbHead);
								if (OrbLaunch) {
									OrbLaunch = false;
									ColliderColor = 0;
									OrbCreate(OrbHead, OrbType, true, 0, 0, 0.25);
								}
							}
						}

						for (int i = 0; i < 7; i++)
						{
							if (Player[i].Online) {
								Player[i].Reflector = ReflectorProcess(Player[i].Reflector, (GameStatus == 1));
							}
						}

						//이 아래는 클라이언트 사이드

						if (Time_Server > Time + 30) Time = Time_Server;
						else if (Time_Server > Time) Time += 2;
						else Time++;

						//Player[0].Reflector = ReflectorControl(Player[0].Reflector, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));

						Player[0].Reflector = ReflectorPosition(Player[0].Reflector, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));

						break;
					case 1:
						if (Reactor.meltdown == false and Orbcount < 0) {
							Orbcount = 0;
							GameStatus = -2;
							//게임오버
						}
						else {
							//협동
						}
						break;
					case 2:
						if (Reactor.meltdown == false and Orbcount < 0) {
							Orbcount = 0;
							GameStatus = -2;
							//게임오버
						}
						else {
							//점수 경쟁
						}
						break;
					case 11:
						//막타 경쟁
						break;
					}
					break;
				}
				break;
			}
			InvalidateRgn(hWnd, NULL, false);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- 위의 세 메시지 외의 나머지 메시지는 OS로
}


// 소켓 관련 메시지 처리
void ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 데이터 통신에 사용할 변수
	int retval;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	SOCKETINFO* ptr;

	// 오류 발생 여부 확인
	if (WSAGETSELECTERROR(lParam)) {
		err_display(WSAGETSELECTERROR(lParam));
		RemoveSocketInfo(wParam);
		return;
	}

	// 메시지 처리
	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_ACCEPT:
		addrlen = sizeof(clientaddr);
		client_sock = accept(wParam, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			return;
		}
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
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
		// 데이터 받기
		retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			RemoveSocketInfo(wParam);
			return;
		}
		ptr->recvbytes = retval;
		// 받은 데이터 출력
		ptr->buf[retval] = '\0';
		addrlen = sizeof(clientaddr);
		getpeername(wParam, (struct sockaddr*)&clientaddr, &addrlen);
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), ptr->buf);
	case FD_WRITE:
		ptr = GetSocketInfo(wParam);
		if (ptr->recvbytes <= ptr->sendbytes)
			return;
		// 데이터 보내기
		retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
			ptr->recvbytes - ptr->sendbytes, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			RemoveSocketInfo(wParam);
			return;
		}
		ptr->sendbytes += retval;
		// 받은 데이터를 모두 보냈는지 체크
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

// 소켓 정보 추가
bool AddSocketInfo(SOCKET sock)
{
	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return false;
	}
	// 단일 연결 리스트에 정보 추가
	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->recvdelayed = false;
	ptr->next = SocketInfoList;
	SocketInfoList = ptr;
	return true;
}

// 소켓 정보 얻기
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

// 소켓 정보 제거
void RemoveSocketInfo(SOCKET sock)
{
	// 클라이언트 정보 출력
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (struct sockaddr*)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// 단일 연결 리스트에서 정보 제거
	SOCKETINFO* curr = SocketInfoList;
	SOCKETINFO* prev = NULL;
	while (curr) {
		if (curr->sock == sock) {
			if (prev)
				prev->next = curr->next;
			else
				SocketInfoList = curr->next;
			closesocket(curr->sock); // 소켓 닫기
			delete curr;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}


