/*
#include "Power_ing.h"
#include "Common.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

void DisplayError(const char* msg);
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ

	

double window_size = 0.96 * 0.8;
bool RestartPressure = false, ChargedMod = false;

int PlayerRGB = RGBTemplate_Yellow;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Power.ing";
LPCTSTR lpszWindowName = L"Power.ing";

HDC hdc, memdc;
HBITMAP hBitmap;


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
	if (!RegisterClassEx(&WndClass)) return 1;

	HWND hWnd = CreateWindow(lpszClass, lpszWindowName, 
		WS_POPUP, 0, 0, (int)(window_size_x), (int)(window_size_y),
		NULL, (HMENU)NULL, hInstance, NULL);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//---------------------------------------------------------------------------//

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);
	
	//---------------------------------------------------------------------------//

	MSG Message;
	while (GetMessage(&Message, 0, 0, 0) > 0) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	WSACleanup();
	return Message.wParam;
}



DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� ����
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ������ ���
	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ���

		// ���ڿ� ���̰� 0�̸� ������ ����
		if (strlen(buf) == 0) {
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸�
			continue;
		}

		// ������ ������

		if ((int)strlen(buf) < 17)
			retval = send(sock, buf, 17, 0);
		else
			retval = send(sock, buf, (int)strlen(buf), 0);

		if (retval == SOCKET_ERROR) {
			DisplayError("send()");
			break;
		}
		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\r\n", retval);

		// ������ �ޱ�
		retval = recv(sock, buf, retval, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			DisplayError("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\r\n", retval);
		DisplayText("[���� ������] %s\r\n", buf);

		EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
		SetEvent(hReadEvent); // �б� �Ϸ� �˸�
	}

	return 0;
}
*/