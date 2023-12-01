#include "Power_ing.h"

int Menu = 0;

double window_size = 0.96 * 0.8;
bool WASD = false, RestartPressure = false, ChargedMod = false, keyboard = true;
int Reflector1Left = 0x25;
int Reflector1Right = 0X27;
int Reflector1Up = 0X26;
int Reflector1Down = 0X28;

int PlayerRGB = RGBTemplate_Yellow;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Power.ing";
LPCTSTR lpszWindowName = L"Power.ing";

HDC hdc, memdc;
HBITMAP hBitmap;

#define BUFSIZE		1024
#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000

// ���� ��� ������ �Լ�
// DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevlnstance, LPSTR lpszCmdParam, int nCmdShow)
{
	g_hInst = hInstance;

	WNDCLASSEX WndClass;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&WndClass);

	HWND hWnd = CreateWindow(lpszClass, lpszWindowName, 
		WS_POPUP, 0, 0, (int)(window_size_x), (int)(window_size_y),
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���� ��� ������ ����
	//CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	MSG Message;
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

//DWORD WINAPI ClientMain(LPVOID arg)
//{
//	int retval;
//
//	// ���� �ʱ�ȭ
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//		return 1;
//
//	// ���� ����
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock == INVALID_SOCKET) 
//		err_quit("socket()");
//
//	// connect()
//	struct sockaddr_in serveraddr;
//	memset(&serveraddr, 0, sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
//	serveraddr.sin_port = htons(SERVERPORT);
//	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) 
//		err_quit("connect()");
//
//	// ������ ��ſ� ����� ����
//	SOCKET client_sock;
//	struct sockaddr_in clientaddr;
//	int addrlen;
//	HANDLE hThread;
//	int clientID = 0;
//
//	// ������ ������ ���
//	while (1) {
//		// ������ ������
//		retval = SendC2SPacket(client_sock, clientID);
//		if (retval == SOCKET_ERROR) {
//			err_quit("send()");
//			break;
//		}
//
//		// ������ �ޱ�
//		retval = RecvS2CPacket(client_sock, clientID);
//		if (retval == SOCKET_ERROR) {
//			err_quit("recv()");
//			break;
//		}
//		else if (retval == 0)
//			break;
//	}
//	// ���� �ݱ�
//	closesocket(client_sock);
//	return 0;
//}
