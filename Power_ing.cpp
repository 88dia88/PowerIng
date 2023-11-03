#include "Power_ing.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//--------------------------------------------------------------------------------------------------------------//
	srand((int)time(NULL));
	PAINTSTRUCT ps;

	POINTS Mouse = MAKEPOINTS(lParam);

	static bool Ingame = false;
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

	static bool Shift = false, Ctrl = false;
	static int CustomRGB[4] = { RGBTemplate_Yellow, 255, 255, 0 };

	int MassSel = 1;
	int rgb[9] = { RGBTemplate_Green, 
		RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green,
		RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan };
	// -------------------



	//PlgBlt = 회전하기 위한 3개(좌상,우상,좌하)의 좌표 필요

	if (iMsg == WM_CREATE) {
		GameStart = false;
		EffectHead->next = EffectHead;
		OrbHead->next = OrbHead;
		ReflectorHead->next = ReflectorHead;
		ReflectorHead = ReflectorReset(ReflectorHead);
		GeneralReset();
		Temperture = Kelvin, Mole = MaxMole * 0.5;
		PreTime = -25;
		Orbcount = 3;
		TotalScore = 0;
		DisplayLoad();
		DisplayColorApply();


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
					ReflectClear();
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
		case WM_MOUSEMOVE:
			switch (GameStatus) {
			case 0:
				if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(-700, 315, 750, 150, Mouse)) SelectedButton = 4;
				else SelectedButton = 0;
				break;
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
					ReflectClear();
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
							ReflectClear();
							break;
						}
					}
					SelectedButton = 0;
				}
				break;
			}
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
							AnimationTime_Door == 100;
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
									OrbCreate(OrbHead, OrbType, true, 0, 0, 0.25);
								}
							}
						}
						ReflectorPosition(ReflectorHead, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));
						ReflectorProcess(ReflectorHead, (GameStatus == 1));
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

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			//-----------------------------------------------------------------------------------------------------------------------------//

			memdc = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, (int)window_size_x, (int)window_size_y);

			SelectObject(memdc, hBitmap);
			SetStretchBltMode(memdc, COLORONCOLOR);
			SetBkMode(memdc, TRANSPARENT);

			if (DisplayGame) {
				//게임 오브젝트 렌더
				//리엑터
				ReactorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
				//레일
				Reactor_RailImg.Draw(memdc, Pibot_x - 782 * window_half, Pibot_y - 782 * window_half, 782 * window_size, 782 * window_size, 0, 0, 782, 782);
				//패널
				DisplayReflector(ReflectorHead);
				//오브
				DisplayOrb(OrbHead);
				//리엑터 이펙트
				
				if (Reactor.meltdownlevel > 0) {
					if (Reactor.meltdownlevel < 72) Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * (int)(Reactor.meltdownlevel / 12), Reactor_size, Reactor_size, Reactor_size);
					else Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * 5, Reactor_size, Reactor_size, Reactor_size);
				}
				else if (Reactor.cherenkovlevel > 0) {
					if (Reactor.cherenkovlevel < 72) Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * (int)(Reactor.cherenkovlevel / 12), 0, Reactor_size, Reactor_size);
					else Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * 5, 0, Reactor_size, Reactor_size);
				}
				//수정 필요 GameStatus 2
				if (Time >= 20 && Time <= 35) Orb_Animation_Img.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * int(Time / 2 - 10), 0, Reactor_size, Reactor_size);
				//외곽
				//조작판
				//버튼
				Cherenkov_LeverImg.Draw(memdc, Pibot_x - (740 * window_size), Pibot_y - (100 * window_size), 200 * window_size, 200 * window_size, 200 * Button[0], 0, 200, 200);
				Button_PressureImg.Draw(memdc, Pibot_x - (861.5 * window_size), Pibot_y + (260 * window_size), 80 * window_size, 80 * window_size, 80 * (PNcmp(Button[1]) + 1), 0, 80, 80);
				Button_PressureImg.Draw(memdc, Pibot_x - (718.5 * window_size), Pibot_y + (260 * window_size), 80 * window_size, 80 * window_size, 80 * (PNcmp(Button[2]) + 1), 0, 80, 80);
				Button_OrbImg.Draw(memdc, Pibot_x + (653.5 * window_size), Pibot_y + (251 * window_size), 50 * window_size, 50 * window_size, 50 * (int)(Button[3] / 3), 0, 50, 50);
				Button_OrbImg.Draw(memdc, Pibot_x + (796.5 * window_size), Pibot_y + (251 * window_size), 50 * window_size, 50 * window_size, 50 * (int)(Button[4] / 3), 50, 50, 50);
				//		수정 필요 GameStatus 2
				if (Time == 0) Button_LampImg.Draw(memdc, Pibot_x + 250 * window_size, Pibot_y + 550 * window_size, 120 * window_size, 120 * window_size, 120 - 120 * PressureCheck(), 0, 120, 120);

				//UI
				DisplayRotatedImage(-860, 0, 20, 110, Reactor.cherenkovmeter / 1500.0 - 1.0 / 3.0, 1);
				DisplayRotatedImage(-700, -405, 14, 80, PressureCaculate(Mole, Temperture), 2);
				DisplayRotatedImage(-821.5, 300, 80, 80, (Temperture - Kelvin) / (MaxTemp - Kelvin), 3);
				DisplayRotatedImage(-678.5, 300, 80, 80, Mole / MaxPressure * Kelvin / 3 - 1.0 / 6.0, 4);
				DisplayRotatedImage(-825, -355, 15, 190, 0, 5);

				//오브 선택 구버전
				//if (Orbselect_scene) OrbImg.Draw(memdc, int(Pibot_x + 700 * window_size), int(Pibot_y - 380 * window_size), int(100 * window_size), int(100 * window_size), 500, Orb_size * OrbType, Orb_size, Orb_size);
				
				UIScore();
				EffectPrint(EffectHead);

				if (debug)	UIDebugInfo();
				else UIGameStatusDebugInfo(GameStatus, EscMode, DisplayGame);

				if (AnimationTime_Door != 0) {
					DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y,
						Controllroom_size_x * ((100 - AnimationTime_Door) / 4 % 5), Controllroom_size_y * (int)((100 - AnimationTime_Door) / 4 / 5),
						Controllroom_size_x, Controllroom_size_y); // 3000 * (PreTime % 5), 2000 * (int)(PreTime / 5)
				}
			}
			else {
				DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y,
					0, 0, Controllroom_size_x, Controllroom_size_y);
				if (GameStatus == -2) UIEndMessage();
				else if (GameStatus == 0) {
					rgb[0] = RGBTemplate_Cyan;
					rgb[4] = RGBTemplate_Yellow;
					rgb[8] = RGBTemplate_Red;
					UIMenu(SelectedButton, L"o ESC\\", L"", L"Resume", L"Options", L"Quit To Main Menu", rgb);
				}
			}

			BitBlt(hdc, 0, 0, (int)window_size_x, (int)window_size_y, memdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
			DeleteDC(memdc);

			//-----------------------------------------------------------------------------------------------------------------------------//

			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
	}
	else {
		switch (iMsg) {
		case WM_KEYDOWN:
			if (wParam == VK_SHIFT) {
				Shift = true;
			}
			else if (wParam == VK_CONTROL) {
				Ctrl = true;
			}
			else if (wParam == VK_ESCAPE) {
				if (Menu_Type == 1) Menu_Type = 0;
				else if (Menu_Type == 2) Menu_Type = 0;
				else if (Menu_Type == 3) Menu_Type = 0;
				else if (Menu_Type / 10 == 2) Menu_Type = 2;
				else if (Menu_Type / 10 == 3) Menu_Type = 3;
				else if (Menu_Type / 10 == 23) Menu_Type = 23;
				else Menu_Type = 0;
			}
			break;
		case WM_KEYUP:
			if (wParam == VK_SHIFT) {
				Shift = false;
			}
			else if (wParam == VK_CONTROL) {
				Ctrl = false;
			}
			break;
		case WM_MOUSEMOVE:
			Mouse = MAKEPOINTS(lParam);
			switch (Menu_Type) {
			case 0:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(-700, 315, 750, 150, Mouse)) SelectedButton = 4;
				else SelectedButton = 0;
				break;
			case 1:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 2:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 3:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 21:
				if (UIButtonSelected(-700, 315, 750, 150, Mouse)) SelectedButton = 4;
				else if (UIButtonSelected(200, -135, 100, 150, Mouse)) SelectedButton = 5;
				else if (UIButtonSelected(750, -135, 100, 150, Mouse)) SelectedButton = 6;
				else if (UIButtonSelected(200, 15, 100, 150, Mouse)) SelectedButton = 7;
				else if (UIButtonSelected(750, 15, 100, 150, Mouse)) SelectedButton = 8;
				else if (UIButtonSelected(200, 165, 100, 150, Mouse)) SelectedButton = 9;
				else if (UIButtonSelected(750, 165, 100, 150, Mouse)) SelectedButton = 10;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 23:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(-700, 315, 750, 150, Mouse)) SelectedButton = 4;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 31:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 32:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 33:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 15, 750, 150, Mouse)) SelectedButton = 2;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			case 231: case 232: case 233: case 234:
				if (UIButtonSelected(-700, -135, 750, 150, Mouse)) SelectedButton = 1;
				else if (UIButtonSelected(-700, 165, 750, 150, Mouse)) SelectedButton = 3;
				else if (UIButtonSelected(200, -500, 750, 150, Mouse)) SelectedButton = -1;
				else SelectedButton = 0;
				break;
			}
			break;
		case WM_LBUTTONDOWN:
			if (SelectedButton == -1) {
				if (Menu_Type == 1) Menu_Type = 0;
				else if (Menu_Type == 2) Menu_Type = 0;
				else if (Menu_Type == 3) Menu_Type = 0;
				else if (Menu_Type / 10 == 2) Menu_Type = 2;
				else if (Menu_Type / 10 == 3) Menu_Type = 3;
				else if (Menu_Type / 10 == 23) Menu_Type = 23;
				else Menu_Type = 0;
				SelectedButton = 0;
			}
			else {
				switch (Menu_Type) {
				case 0:
					if (SelectedButton == 4) PostQuitMessage(0);
					else Menu_Type = SelectedButton;
					break;
				case 1:
					if (SelectedButton != 0) {
						Orbcount = 3;
						EscMode = 0;
						GameMode = 0;
						GameStatus = -1;
						ReflectorCreate(ReflectorHead, 0);
						SelectedButton = 0;
						DisplayGame = true;
						Ingame = true;
					}
					break;
				case 2:
					Menu_Type = 20 + SelectedButton;
					SelectedButton = 0;
					break;
				case 3:
					Menu_Type = 30 + SelectedButton;
					break;
				case 21:
					if (Ctrl) MassSel *= 10;
					if (Shift) MassSel *= 5;
					switch (SelectedButton) {
					case 4:
						CustomRGB[0] = (CustomRGB[1] << 16) | (CustomRGB[2] << 8) | CustomRGB[3];
						PlayerRGB = CustomRGB[0];
						DisplayColorApply();
						Menu_Type = 2;
						break;
					case 5:
						if (CustomRGB[1] > 0) {
							if (CustomRGB[1] > MassSel) CustomRGB[1] -= MassSel;
							else CustomRGB[1] = 0;
						}
						break;
					case 6:
						if (CustomRGB[1] < 255) {
							if (CustomRGB[1] < 255 - MassSel) CustomRGB[1] += MassSel;
							else CustomRGB[1] = 255;
						}
						break;
					case 7:
						if (CustomRGB[2] > 0) {
							if (CustomRGB[2] > MassSel) CustomRGB[2] -= MassSel;
							else CustomRGB[2] = 0;
						}
						break;
					case 8:
						if (CustomRGB[2] < 255) {
							if (CustomRGB[2] < 255 - MassSel) CustomRGB[2] += MassSel;
							else CustomRGB[2] = 255;
						}
						break;
					case 9:
						if (CustomRGB[3] > 0) {
							if (CustomRGB[3] > MassSel) CustomRGB[3] -= MassSel;
							else CustomRGB[3] = 0;
						}
						break;
					case 10:
						if (CustomRGB[3] < 255) {
							if (CustomRGB[3] < 255 - MassSel) CustomRGB[3] += MassSel;
							else CustomRGB[3] = 255;
						}
						break;
					}
					CustomRGB[0] = (CustomRGB[1] << 16) | (CustomRGB[2] << 8) | CustomRGB[3];
					break;
				case 23:
					Menu_Type = 230 + SelectedButton;
					break;
				case 31:
					break;
				case 32:
					break;
				case 33:
					break;
				case 231: case 232: case 233: case 234:
					int SelectedModule = Menu_Type % 230;
					if (SelectedButton == 1) {
						if (ReflectorHead->module[SelectedModule] == 1) ReflectorHead->module[SelectedModule] = 0;
						else ReflectorHead->module[SelectedModule] = 1;
					}
					else if (SelectedButton == 3) {
						if (ReflectorHead->module[SelectedModule] == 2) ReflectorHead->module[SelectedModule] = 0;
						else ReflectorHead->module[SelectedModule] = 2;
					}
					break;
				}
			}
			break;
		case WM_TIMER:
			InvalidateRgn(hWnd, NULL, false);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			//-----------------------------------------------------------------------------------------------------------------------------//
			memdc = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, (int)window_size_x, (int)window_size_y);

			SelectObject(memdc, hBitmap);
			SetStretchBltMode(memdc, COLORONCOLOR);
			SetBkMode(memdc, TRANSPARENT);

			DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
			//UIMenu(start_button, module_button, option_button, quit_button, FALSE);

			if (Menu_Type == 0) {
				rgb[4] = RGBTemplate_Yellow;
				rgb[8] = RGBTemplate_Red;
				UIMenu(SelectedButton, L"Power Ing\\", L"Start!", L"Custom", L"Options", L"Quit", rgb);
			}
			else if (Menu_Type == 1) {
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIMenu(SelectedButton, L"Game Mode", L"Co-Op", L"HS-Competive", L"Competive", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 2) {
				rgb[1] = PlayerRGB;
				rgb[2] = RGBTemplate_Gray;
				rgb[5] = PlayerRGB;
				rgb[6] = RGBTemplate_Gray;
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIModule(400, 100, true);
				UIModule(400, -100, false);
				UIMenu(SelectedButton, L"Module", L"Color", L"", L"Module", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 3) {
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIMenu(SelectedButton, L"Options", L"GamePlay", L"Display", L"Sound", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 21) {
				rgb[1] = RGBTemplate_Red;
				rgb[2] = RGBTemplate_Green;
				rgb[3] = RGBTemplate_Blue;
				rgb[5] = RGBTemplate_Red;
				rgb[6] = RGBTemplate_Green;
				rgb[7] = RGBTemplate_Blue;
				rgb[4] = CustomRGB[0];
				rgb[8] = CustomRGB[0];

				UIModule(350, 400, SelectedButton == 4);
				UIRGBSlider(CustomRGB[1], CustomRGB[2], CustomRGB[3]);
				UIMenu(SelectedButton, L"Color", L"Red", L"Green", L"Blue", L"Apply", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 23) {
				UIModule(400, 100, true);
				UIModule(400, -100, false);
				UIMenu(SelectedButton, L"Module", L"Movement", L"Energy", L"Reactive", L"Divider", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 31) {
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIMenu(SelectedButton, L"GamePlay", L"Control", L"Reset Speed Setting?", L"Score", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 32) {
				rgb[3] = RGBTemplate_Gray;
				rgb[7] = RGBTemplate_Gray;
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIMenu(SelectedButton, L"Display", L"Resolution", L"Trail Quality", L"", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 33) {
				rgb[4] = RGBTemplate_Gray;
				rgb[8] = RGBTemplate_Gray;
				UIMenu(SelectedButton, L"Sound", L"Master Volum", L"Volum", L"Alert Setting", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type / 10 == 23) {
				int SelectedModule = Menu_Type % 230;
				if (ReflectorHead->module[SelectedModule] == 1) {
					rgb[1] = RGBTemplate_Red;
					rgb[5] = RGBTemplate_Yellow;
				}
				else if (ReflectorHead->module[SelectedModule] == 2) {
					rgb[3] = RGBTemplate_Red;
					rgb[7] = RGBTemplate_Yellow;
				}
				UIModule(400, 100, true);
				UIModule(400, -100, false);
				if (SelectedModule == 1) UIMenu(SelectedButton, L"Movement", L"Gear", L"", L"Magnet", L"", rgb);
				else if (SelectedModule == 2) UIMenu(SelectedButton, L"Energy", L"Electric", L"", L"Cherenkov", L"", rgb);
				else if (SelectedModule == 3) UIMenu(SelectedButton, L"Reactive", L"Acceleration", L"", L"Decceleration", L"", rgb);
				else if (SelectedModule == 4) UIMenu(SelectedButton, L"Divider", L"Red", L"", L"Blue", L"", rgb);
				UIBack(SelectedButton == -1);
			}

			BitBlt(hdc, 0, 0, (int)window_size_x, (int)window_size_y, memdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
			DeleteDC(memdc);

			//-----------------------------------------------------------------------------------------------------------------------------//

			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
	}
	//if (Menu == 0) return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- 위의 세 메시지 외의 나머지 메시지는 OS로
	//else return 0;
	return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- 위의 세 메시지 외의 나머지 메시지는 OS로
}



/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//--------------------------------------------------------------------------------------------------------------//
	srand((int)time(NULL));
	PAINTSTRUCT ps;

	POINTS Mouse = MAKEPOINTS(lParam);

	// 추가변수--------
	static bool Endscene = false;		// 엔드씬
	static bool Orbselect_scene = false;
	static bool Start_scene = true;		// 스타트 씬
	static bool EscMode = false;
	static bool start_button = false;	// 각 버튼위에 마우스가 올라가 있으면 값이 변경됨
	static bool module_button = false;
	static bool option_button = false;
	static bool quit_button = false;
	// -------------------

	//PlgBlt = 회전하기 위한 3개(좌상,우상,좌하)의 좌표 필요

	switch (iMsg) {
	case WM_CREATE:
		GameStart = false;
		EffectHead->next = EffectHead;
		OrbHead->next = OrbHead;
		ReflectorHead->next = ReflectorHead;
		ReflectorHead = ReflectorReset(ReflectorHead);
		GeneralReset();
		Temperture = Kelvin, Mole = MaxMole * 0.5;
		PreTime = -25;
		Orbcount = 3;
		TotalScore = 0;
		DisplayLoad();
		DisplayColorApply();

		SetTimer(hWnd, 5, 50, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (EscMode)
			{
				EscMode = false;
				Start_scene = false;
			}
			else if (Endscene)
			{
				SetTimer(hWnd, 5, 50, NULL);
			}
			else if (Start_scene == false)
			{
				EscMode = true;
				PreTime = 15;
			}
		}
		if (wParam == VK_RETURN) {
			if (GameStart)
			{
				if (Cherenkov.lever == 0 || Cherenkov.meter >= 875 && Cherenkov.lever < 6 && Cherenkov.cherenkov == false)
				{
					Cherenkov.levertrigger = true;
					Cherenkov.lever++;
				}
			}
			else if (PressureCheck())
			{
				if (Orbselect_scene)
				{
					if (OrbType == 1) Orbcount = 5;
					else  Orbcount = 3;
					Orbselect_scene = false;
				}
				Time = 0;
				GameStart = true;
			}
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_RETURN) {
			Cherenkov.levertrigger = false;
		}
		break;
	case WM_MOUSEMOVE:
		if (Start_scene)
		{
			Mouse = MAKEPOINTS(lParam);
			start_button = (!EscMode && UIButtonSelected(-700, -135, 750, 150, Mouse));
			module_button = UIButtonSelected(-700, 15, 750, 150, Mouse);
			option_button = UIButtonSelected(-700, 165, 750, 150, Mouse);
			quit_button = UIButtonSelected(-700, 315, 750, 150, Mouse);
		}
		else if (Time == 0)
		{
			if (Button[1] != 0)
			{
				Mouse = MAKEPOINTS(lParam);
				if (Mouse.x < Pibot_x - 178.5 * window_size)
				{
					if (Mouse.x > Pibot_x - 821.5 * window_size) Button[1] = (int)(DistancePosition(Mouse.x - (Pibot_x - 821.5 * window_size), Mouse.y - (Pibot_x - 300 * window_size)) / 12.5) + 1;
					else Button[1] = -(int)(DistancePosition(Mouse.x - (Pibot_x - 821.5 * window_size), Mouse.y - (Pibot_x - 300 * window_size)) / 12.5) - 1;
				}
			}
			if (Button[2] != 0)
			{
				Mouse = MAKEPOINTS(lParam);
				if (Mouse.x < Pibot_x - 321.5 * window_size)
				{
					if (Mouse.x > Pibot_x - 678.5 * window_size) Button[2] = (int)(DistancePosition(Mouse.x - (Pibot_x - 678.5 * window_size), Mouse.y - (Pibot_x - 300 * window_size)) / 25) + 1;
					else Button[2] = -(int)(DistancePosition(Mouse.x - (Pibot_x - 678.5 * window_size), Mouse.y - (Pibot_x - 300 * window_size)) / 25) - 1;
				}
			}
		}
		else if (Cherenkov.levertrigger)
		{
			if (Cherenkov.lever == 0 || Cherenkov.meter >= 875 && Cherenkov.lever < 6 && Cherenkov.cherenkov == false)
			{
				if (UIButtonSelected(-737.5, -67.5, 200, 167.5, Mouse) && Cherenkov.lever == 0) Cherenkov.lever = 1;
				else if (UIButtonSelected(-737.5, -42.5, 200, 142.5, Mouse) && Cherenkov.lever == 1) Cherenkov.lever = 2;
				else if (UIButtonSelected(-737.5, -15, 200, 115, Mouse) && Cherenkov.lever == 2) Cherenkov.lever = 3;
				else if (UIButtonSelected(-737.5, 15, 200, 85, Mouse) && Cherenkov.lever == 3) Cherenkov.lever = 4;
				else if (UIButtonSelected(-737.5, 42.5, 200, 57.5, Mouse) && Cherenkov.lever == 4) Cherenkov.lever = 5;
				else if (UIButtonSelected(-737.5, 67.5, 200, 32.5, Mouse) && Cherenkov.lever == 5) Cherenkov.lever = 6;
			}
		}
		else if (keyboard == false)
		{
			Mouse = MAKEPOINTS(lParam);
			//Reflector->angle = AnglePosition(Mouse.x - Pibot_x, Mouse.y - Pibot_y);
		}
		break;
	case WM_LBUTTONDOWN:
		if (Start_scene)
		{
			if (start_button) {
				Start_scene = false;
				PreTime = -25;
			}
			else if (module_button) {
				if (EscMode)
				{
					EscMode = false;
					Start_scene = false;
				}
				else
				{
					// 모듈 클릭시 대화상자
					DialogBoxW(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)&Module_Proc);
				}
			}
			else if (option_button)
			{
				// 옵션 클릭시 대화상자
				DialogBoxW(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)&Option_Proc);
			}
			else if (quit_button)
			{
				if (EscMode)
				{
					SetTimer(hWnd, 5, 50, NULL);
				}
				else PostQuitMessage(0);
			}
		}
		else if (Endscene)
		{
			SetTimer(hWnd, 5, 50, NULL);
		}
		else if (Time == 0)
		{
			Mouse = MAKEPOINTS(lParam);
			if (UIButtonSelected(-821.5, 260, 40, 80, Mouse)) Button[1] = 1;
			else if (UIButtonSelected(-861.5, 260, 40, 80, Mouse)) Button[1] = -1;
			else if (UIButtonSelected(-678.5, 260, 40, 80, Mouse)) Button[2] = 1;
			else if (UIButtonSelected(-718.5, 260, 40, 80, Mouse)) Button[2] = -1;
			if (Orbselect_scene)
			{
				if (UIButtonSelected(653.5, 250, 50, 50, Mouse))
				{
					OrbType--;
					if (OrbType < 0) OrbType = 2;
					Button[3] = 10;
				}
				else if (UIButtonSelected(796.5, 250, 50, 50, Mouse))
				{
					OrbType++;
					if (OrbType > 2) OrbType = 0;
					Button[4] = 10;
				}
			}
		}
		else
		{
			Mouse = MAKEPOINTS(lParam);
			if (UIButtonSelected(-737.5, -100, 200, 200, Mouse)) Cherenkov.levertrigger = true;
			if (UIButtonSelected(653.5, 250, 50, 50, Mouse)) Button[3] = 10;
			else if (UIButtonSelected(796.5, 250, 50, 50, Mouse)) Button[4] = 10;
		}
		break;
	case WM_LBUTTONUP:
		Button[1] = 0;
		Button[2] = 0;
		Cherenkov.levertrigger = false;
		break;
	case WM_TIMER:
		switch (wParam) {
		case 0:				//GetAsyncKeyState - 키가 눌린 상태로 진행되는함수 (끊김없이)http://www.silverwolf.co.kr/cplusplus/4842
			if (EscMode)
			{
				if (PreTime > -5) PreTime--;
				else if (Start_scene == false) Start_scene = true;
			}
			else if (Orbcount < 0)
			{
				Orbcount = 0;
				SetTimer(hWnd, 100, 50, NULL);
			}
			else if (PreTime < 16) PreTime++;
			else if (Time < 0)
			{
				if (ReactorEffect > 6) GameRestart();
				else if (ReactorEffect == 6) GeneralReset();
			}
			else
			{
				if (Time == 0 && PreTime < 25) PreTime = 25;
				ButtonActive();
				ReflectorPosition(ReflectorHead, GetAsyncKeyState(Reflector1Left), GetAsyncKeyState(Reflector1Right), GetAsyncKeyState(Reflector1Up), GetAsyncKeyState(Reflector1Down));
				if (GameStart)
				{
					if (Time == 25)
					{
						Orbcount--;
						OrbCreate(OrbHead, OrbType, true, 0, 0, 0.25);
					}
					Time++;
					CherenkovCheck();
					CollisionDetect(OrbHead);
					if (ReactorEffect > 5)
					{
						if (ReactorEffect < 15)
						{
							if (Time % 3 == 0)	ReactorEffect++;
						}
						else
						{
							TotalScore += Score;
							ReactorEffect = 12;
							Time = -1;
							GameStart = false;
						}
					}
				}
			}
			break;
		case 5:				// 스타트 버튼 누르면 1초뒤에 문열리는 애니메이션
			if (Endscene)
			{
				PreTime = 5;
				TotalScore = 0;
				GeneralReset();
				Endscene = false;
				Start_scene = true;
				ReflectClear();
			}
			else if (EscMode)
			{
				KillTimer(hWnd, 0);
				PreTime = 5;
				TotalScore = 0;
				GeneralReset();
				EscMode = false;
				Start_scene = true;
				ReflectClear();
			}
			else if (Start_scene)
			{
				if (PreTime > -25) PreTime--;
				else if (rand() % 50 == 0) PreTime = 5;
			}
			else
			{
				if (PreTime < 25) PreTime++;
				else
				{
					ReflectorCreate(ReflectorHead, 0);
					Orbselect_scene = true;
					SetTimer(hWnd, 0, 10, NULL);
					KillTimer(hWnd, 5);
				}
			}
			break;
		case 100:			// -----엔드씬 추가부
			if (Endscene)
			{
				if (PreTime > -5) PreTime--;
				else
				{
					PreTime = -25;
					KillTimer(hWnd, 100);
				}
			}
			else
			{
				Endscene = true;
				PreTime = 25;
				KillTimer(hWnd, 0);
			}
			break;
		}

		InvalidateRgn(hWnd, NULL, false);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		//-----------------------------------------------------------------------------------------------------------------------------//

		memdc = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, (int)window_size_x, (int)window_size_y);

		SelectObject(memdc, hBitmap);
		SetStretchBltMode(memdc, COLORONCOLOR);
		SetBkMode(memdc, TRANSPARENT);

		if (Start_scene)
		{
			// 스타트 씬
			DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
			UIMenu(start_button, module_button, option_button, quit_button, EscMode);
			if (PreTime > 0) Door_Light_Img.Draw(memdc, 0, 0, window_size_x, window_size_y, 0, 0, 2000, 1125);
		}
		else if (Endscene && PreTime == -25)
		{
			DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
			UIEndMessage();
		}
		else
		{
			ReactorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
			Reactor_RailImg.Draw(memdc, Pibot_x - 782 * window_half, Pibot_y - 782 * window_half, 782 * window_size, 782 * window_size, 0, 0, 782, 782);

			if (Time == 0) Button_LampImg.Draw(memdc, Pibot_x + 250 * window_size, Pibot_y + 550 * window_size, 120 * window_size, 120 * window_size, 120 - 120 * PressureCheck(), 0, 120, 120);

			DisplayReflector(ReflectorHead);

			if (ReactorEffect < 12) Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * (ReactorEffect % 6), Reactor_size * (int)(ReactorEffect / 6), Reactor_size, Reactor_size);
			else Reactor_EffectImg.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, 5000, 1000, Reactor_size, Reactor_size);

			DisplayOrb(OrbHead);

			if (Time >= 20 && Time <= 35) Orb_Animation_Img.Draw(memdc, Pibot_x - Reactor_half, Pibot_y - Reactor_half, Reactor_window, Reactor_window, Reactor_size * int(Time / 2 - 10), 0, Reactor_size, Reactor_size);

			Cherenkov_LeverImg.Draw(memdc, Pibot_x - (740 * window_size), Pibot_y - (100 * window_size), 200 * window_size, 200 * window_size, 200 * Cherenkov.lever, 0, 200, 200);

			Button_PressureImg.Draw(memdc, Pibot_x - (861.5 * window_size), Pibot_y + (260 * window_size), 80 * window_size, 80 * window_size, 80 * (PNcmp(Button[1]) + 1), 0, 80, 80);
			Button_PressureImg.Draw(memdc, Pibot_x - (718.5 * window_size), Pibot_y + (260 * window_size), 80 * window_size, 80 * window_size, 80 * (PNcmp(Button[2]) + 1), 0, 80, 80);
			Button_OrbImg.Draw(memdc, Pibot_x + (653.5 * window_size), Pibot_y + (251 * window_size), 50 * window_size, 50 * window_size, 50 * (int)(Button[3] / 3), 0, 50, 50);
			Button_OrbImg.Draw(memdc, Pibot_x + (796.5 * window_size), Pibot_y + (251 * window_size), 50 * window_size, 50 * window_size, 50 * (int)(Button[4] / 3), 50, 50, 50);

			DisplayRotatedImage(-860, 0, 20, 110, Cherenkov.meter / 1500.0 - 1.0 / 3.0, 1);
			DisplayRotatedImage(-700, -405, 14, 80, PressureCaculate(Mole, Temperture), 2);
			DisplayRotatedImage(-821.5, 300, 80, 80, (Temperture - Kelvin) / (MaxTemp - Kelvin), 3);
			DisplayRotatedImage(-678.5, 300, 80, 80, Mole / MaxPressure * Kelvin / 3 - 1.0 / 6.0, 4);
			DisplayRotatedImage(-825, -355, 15, 190, 0, 5);

			if (debug)	UIDebugInfo();
			if (Orbselect_scene) OrbImg.Draw(memdc, int(Pibot_x + 700 * window_size), int(Pibot_y - 380 * window_size), int(100 * window_size), int(100 * window_size), 500, Orb_size * OrbType, Orb_size, Orb_size);
			else UIScore();
			EffectPrint(EffectHead);

			// 문 애니메이션 씬
			if (PreTime < 0)
			{
				DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);
				if (int(PreTime * 0.2 + 1) & 1) UIMenu(true, false, false, false, false);
			}
			else if (PreTime <= 25) {
				DoorImg.Draw(memdc, Pibot_x - Controllroom_half_x, Pibot_y - Controllroom_half_y, Controllroom_window_x, Controllroom_window_y,
					Controllroom_size_x * (PreTime % 5), Controllroom_size_y * (int)(PreTime / 5), Controllroom_size_x, Controllroom_size_y); // 3000 * (PreTime % 5), 2000 * (int)(PreTime / 5)
			}
		}
		BitBlt(hdc, 0, 0, (int)window_size_x, (int)window_size_y, memdc, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteDC(memdc);

		//-----------------------------------------------------------------------------------------------------------------------------//

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- 위의 세 메시지 외의 나머지 메시지는 OS로
}

BOOL CALLBACK Module_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	switch (iMsg) {
	case WM_INITDIALOG:
		CheckRadioButton(hDlg, 1001, 1003, 1001);		//1001 부터 1015까지는 라디오 박스 번호
		CheckRadioButton(hDlg, 1004, 1006, 1004);
		CheckRadioButton(hDlg, 1007, 1009, 1007);
		CheckRadioButton(hDlg, 1010, 1012, 1010);
		CheckRadioButton(hDlg, 1013, 1015, 1013);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO1: ReflectorHead->module[0] = 0;
			break;
		case IDC_RADIO2: ReflectorHead->module[0] = 1;
			break;
		case IDC_RADIO3: ReflectorHead->module[0] = 2;
			break;
		case IDC_RADIO4: ReflectorHead->module[1] = 0;
			break;
		case IDC_RADIO5: ReflectorHead->module[1] = 1;
			break;
		case IDC_RADIO6: ReflectorHead->module[1] = 2;
			break;
		case IDC_RADIO7: ReflectorHead->module[2] = 0;
			break;
		case IDC_RADIO8: ReflectorHead->module[2] = 1;
			break;
		case IDC_RADIO9: ReflectorHead->module[2] = 2;
			break;
		case IDC_RADIO10: ReflectorHead->module[3] = 0;
			break;
		case IDC_RADIO11: ReflectorHead->module[3] = 1;
			break;
		case IDC_RADIO12: ReflectorHead->module[3] = 2;
			break;
		case IDC_RADIO13: ReflectorHead->module[4] = 0;
			break;
		case IDC_RADIO14: ReflectorHead->module[4] = 1;
			break;
		case IDC_RADIO15: ReflectorHead->module[4] = 2;
			break;

		case IDOK:

			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return 0;
}
*/









BOOL CALLBACK Option_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	HWND Button1 = GetDlgItem(hDlg, IDC_CHECK1);
	HWND Button2 = GetDlgItem(hDlg, IDC_CHECK2);
	HWND Button3 = GetDlgItem(hDlg, IDC_CHECK3);
	HWND Button4 = GetDlgItem(hDlg, IDC_CHECK4);

	switch (iMsg) {
	case WM_INITDIALOG:
		CheckRadioButton(hDlg, IDC_RADIO20, IDC_RADIO22, IDC_RADIO20);		//1001 부터 1003까지는 라디오 박스 번호
		if (debug == true) SendMessage(Button1, BM_SETCHECK, BST_CHECKED, 0);
		if (WASD == true) SendMessage(Button2, BM_SETCHECK, BST_CHECKED, 0);
		if (RestartPressure == true) SendMessage(Button3, BM_SETCHECK, BST_CHECKED, 0);
		if (ChargedMod == true) SendMessage(Button4, BM_SETCHECK, BST_CHECKED, 0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO20:
			window_size = 0.96;
			break;

		case IDC_RADIO21:
			window_size = 0.96 / 1.25;
			break;

		case IDC_RADIO22:
			window_size = 0.96 / 1.5;
			break;

		case IDC_RADIO23:		// 빨강	 R, G, B
			Player1RGB[0] = 255;
			Player1RGB[1] = 0;
			Player1RGB[2] = 0;
			break;
		case IDC_RADIO24:		// 파랑
			Player1RGB[0] = 0;
			Player1RGB[1] = 0;
			Player1RGB[2] = 255;
			break;
		case IDC_RADIO25:		// 녹색
			Player1RGB[0] = 0;
			Player1RGB[1] = 255;
			Player1RGB[2] = 0;
			break;
		case IDC_RADIO26:		// 자홍
			Player1RGB[0] = 255;
			Player1RGB[1] = 0;
			Player1RGB[2] = 255;
			break;
		case IDC_RADIO27:		// 청록
			Player1RGB[0] = 0;
			Player1RGB[1] = 255;
			Player1RGB[2] = 255;
			break;
		case IDC_RADIO28:		// 노랑
			Player1RGB[0] = 255;
			Player1RGB[1] = 255;
			Player1RGB[2] = 0;
			break;
		case IDOK: //--- 버튼
			if (SendMessage(Button1, BM_GETCHECK, 0, 0) == BST_CHECKED) debug = true;
			else  debug = false;

			if (SendMessage(Button2, BM_GETCHECK, 0, 0) == BST_CHECKED) WASD = true;
			else WASD = false;

			if (SendMessage(Button3, BM_GETCHECK, 0, 0) == BST_CHECKED) RestartPressure = true;
			else RestartPressure = false;

			if (SendMessage(Button4, BM_GETCHECK, 0, 0) == BST_CHECKED) ChargedMod = true;
			else ChargedMod = false;

			if (WASD)
			{
				Reflector1Left = 'a';
				Reflector1Right = 'd';
				Reflector1Up = 'w';
				Reflector1Down = 's';
			}
			else
			{
				Reflector1Left = 0x25;
				Reflector1Right = 0X27;
				Reflector1Up = 0X26;
				Reflector1Down = 0X28;
			}

			//해상도 버튼 - 라디오		현재 1 -> 1,  0.6 , 0.8
			DisplayWindow();		// 이미지 다시 불러오기
			DisplayColorApply();	// 컬러 다시 불러오기
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL: //--- 버튼
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}

	return 0;
}