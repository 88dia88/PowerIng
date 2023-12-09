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

	int MinCount = 3;
	static bool OrbLaunch = false;

	static int CustomRGB[4] = { RGBTemplate_Yellow, 255, 255, 0 };

	int MassSel = 1;
	int rgb[9] = { RGBTemplate_Green, 
		RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green, RGBTemplate_Green,
		RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan, RGBTemplate_Cyan };
	// -------------------



	//PlgBlt = 회전하기 위한 3개(좌상,우상,좌하)의 좌표 필요

	if (iMsg == WM_CREATE) {
		GameStart = false;
		Setting = SettingReset(Setting);
		OrbHead->next = OrbHead;
		Player[0] = PlayerReset(Player[0], 0);
		for (int i = 1; i < 7; i++)
		{
			Player[i] = PlayerReset(Player[i], i);
			Player[i].Reflector.polar_x = i / 6.0;
			//Player[i].Online = false; // 주석 처리하면 추가 패널이 활성화됨
			if (i % 2 == 0) Player[i].Online = false; // 주석 처리하면 6개의 패널이 활성화됨

			 //아래 코드의 주석 처리를 해제하면 플레이어에 9가지중 무작위 색상이 적용됨
			/*
			*/
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
				for (int j = 0; j < i; j++) if (Player[i].RGB == Player[j].RGB) ColorDupe++;
				if (ColorDupe == 0) break;
			}
			
			Player[i].Reflector.RGB = Player[i].RGB;
		}
		GeneralReset();

		Temperture = Kelvin, Mole = MaxMole * 0.5;
		Orbcount = 3;

		AnimationTime_Door = 100;

		/*
		ReflectorHead->next = ReflectorHead;
		ReflectorHead = ReflectorReset(ReflectorHead);
		*/

		SetTimer(hWnd, 0, 1000 / 60, NULL);

		//<디스플레이>--------------------

		EffectHead->next = EffectHead;
		DisplayLoad();
		DisplayOrbLoad();

		DisplayColorApply(Player[0].RGB);
		DisplayReflectorColorApply(Player[0].RGB);

		DisplayPlayerColorApply(Player[0].RGB, 0);

		for (int i = 1; i < 7; i++) DisplayPlayerColorApply(Player[i].RGB, i);
	}
	if (Ingame) {
		switch (iMsg) {
		case WM_KEYDOWN:
			if (GameStatus == -2) {
				if (DisplayGame == false) {
					Menu_Type = 0;
					EscMode = 0;
					Ingame = false;
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
							GeneralReset();
							break;
						}
					}
					SelectedButton = 0;
				}
				break;
			}
			break;
		case WM_TIMER:
			switch (wParam)
			{
			case 0:
				switch (GameStatus) {
				case -2:
					if (DisplayGame) {
						if (AnimationTime_Door < 100) AnimationTime_Door += 2;
						else {
							AnimationTime_Door = 100;
							for (int i = 0; i < 7; i++) {
								if (Player[i].Count < 3) Player[i].Count = 3;
							}
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
						for (int i = 0; i < 7; i++) {
							if (Player[i].Count < MinCount) MinCount = Player[i].Count;
						}

						if (Reactor.meltdown == false and MinCount <= 0) {
							MinCount = 3;
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
								Player[i].Reflector = ReflectorPosition(Player[i].Reflector,
									Player[i].Control[1], Player[i].Control[2], Player[i].Control[3], Player[i].Control[4]);

								if (Reactor.cherenkov == false) {
									if (Player[i].Reflector.cherenkovcounter > 0)
									{
										if (Player[i].CherenkovMeter < 1000) Player[i].CherenkovMeter++;
										Player[i].Reflector.cherenkovcounter--;
									}
								}	
								if (Player[i].Control[0] & 0x8001 || Player[i].Control[0] & 0x8000) {
									if (Reactor.cherenkov == false) {
										if (Player[i].CherenkovMeter == 1000) {
											Reactor.cherenkov = true;
											Reactor.cherenkovmeter = Player[i].CherenkovMeter;
											Player[i].CherenkovMeter = 0;

										}
										else if (Player[i].CherenkovMeter >= 8750) {
											//완전 충전 되지 않았으면 꾹 눌러서 발동
											Reactor.cherenkov = true;
											Reactor.cherenkovmeter = Player[i].CherenkovMeter;
											Player[i].CherenkovMeter = 0;
										}
									}
								}
							}
						}

						


						//이 아래는 클라이언트 사이드

						if (Time_Server > Time + 30) Time = Time_Server;
						else if (Time_Server > Time) Time += 2;
						else Time++;

						if (Player[0].CherenkovMeter == 1000 and Setting.Game_Cherenkov_auto) Player[0].Control[0] = 0x8001;
						else Player[0].Control[0] = GetAsyncKeyState(Setting.Control_Active);
						Player[0].Control[1] = GetAsyncKeyState(Setting.Control_Left);
						Player[0].Control[2] = GetAsyncKeyState(Setting.Control_Right);
						Player[0].Control[3] = GetAsyncKeyState(Setting.Control_Up);
						Player[0].Control[4] = GetAsyncKeyState(Setting.Control_Down);



						/*컨트롤 테스트
						for (int i = 1; i < 7; i++)
						{
							if (Player[i].Online) {
								Player[i].Control[0] = GetAsyncKeyState(Setting.Control_Active);
							Player[i].Control[1] = GetAsyncKeyState(Setting.Control_Left);
							Player[i].Control[2] = GetAsyncKeyState(Setting.Control_Right);
							Player[i].Control[3] = GetAsyncKeyState(Setting.Control_Up);
							Player[i].Control[4] = GetAsyncKeyState(Setting.Control_Down);
							}
						}
						*/


						break;
					case 1:
						if (Reactor.meltdown == false and Player[0].Count < 0) {
							Orbcount = 0;
							GameStatus = -2;
							//게임오버
						}
						else {
							//협동
						}
						break;
					case 2:
						if (Reactor.meltdown == false and Player[0].Count < 0) {
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
				ReactorImg.Draw(memdc, int(Pibot_x - Controllroom_half_x), int(Pibot_y - Controllroom_half_y), int(Controllroom_window_x), int(Controllroom_window_y), 0, 0, Controllroom_size_x, Controllroom_size_y);
				//레일
				Reactor_RailImg.Draw(memdc, int(Pibot_x - 782 * window_half), int(Pibot_y - 782 * window_half), int(782 * window_size), int(782 * window_size), 0, 0, 782, 782);
				//패널

				for (int i = 0; i < 7; i++)
				{
					if (Player[i].Online) {
						//DisplayReflector(Player[i].Reflector);
						DisplayPlayerReflector(Player[i].Reflector, i);
					}
				}

				//오브
				DisplayOrb(OrbHead);
				//리엑터 이펙트

				if (ColliderColor != 0) {
					if (Reactor.meltdownlevel > 0) {
						if (Reactor.meltdownlevel < 72) 
							Reactor_Effect_ColorImg[ColliderColor].Draw(memdc, 
								int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
								Reactor_window, Reactor_window, 
								Reactor_size * (int)(Reactor.meltdownlevel / 12), 0, 
								Reactor_size, Reactor_size);
						else 
							Reactor_Effect_ColorImg[ColliderColor].Draw(memdc, 
								int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
								Reactor_window, Reactor_window, 
								Reactor_size * 5, 0, 
								Reactor_size, Reactor_size);
					}
				}
				else if(Reactor.meltdownlevel > 0) {
					if (Reactor.meltdownlevel < 72) 
						Reactor_EffectImg.Draw(memdc, 
							int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
							Reactor_window, Reactor_window, 
							Reactor_size * (int)(Reactor.meltdownlevel / 12), Reactor_size, 
							Reactor_size, Reactor_size);
					else 
						Reactor_EffectImg.Draw(memdc, 
							int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
							Reactor_window, Reactor_window, 
							Reactor_size * 5, Reactor_size, 
							Reactor_size, Reactor_size);
				}
				else if (Reactor.cherenkovlevel > 0) {
					if (Reactor.cherenkovlevel < 72) 
						Reactor_EffectImg.Draw(memdc, 
							int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
							Reactor_window, Reactor_window, 
							Reactor_size * (int)(Reactor.cherenkovlevel / 12), 0, 
							Reactor_size, Reactor_size);
					else 
						Reactor_EffectImg.Draw(memdc, 
							int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), 
							Reactor_window, Reactor_window, 
							Reactor_size * 5, 0, 
							Reactor_size, Reactor_size);
				}
				//수정 필요 Orb 시작 애니메이션

				//if (Time >= 20 && Time <= 35) Orb_Animation_Img.Draw(memdc, int(Pibot_x - Reactor_half), int(Pibot_y - Reactor_half), Reactor_window, Reactor_window, Reactor_size * int(Time / 2 - 10), 0, Reactor_size, Reactor_size);
				
				//외곽
				//조작판
				//버튼
				Cherenkov_LeverImg.Draw(memdc, int(Pibot_x - (740 * window_size)), int(Pibot_y - (100 * window_size)), int(200 * window_size), int(200 * window_size), 200 * Button[0], 0, 200, 200);
				Button_PressureImg.Draw(memdc, int(Pibot_x - (861.5 * window_size)), int(Pibot_y + (260 * window_size)), int(80 * window_size), int(80 * window_size), 80 * (PNcmp(Button[1]) + 1), 0, 80, 80);
				Button_PressureImg.Draw(memdc, int(Pibot_x - (718.5 * window_size)), int(Pibot_y + (260 * window_size)), int(80 * window_size), int(80 * window_size), 80 * (PNcmp(Button[2]) + 1), 0, 80, 80);
				Button_OrbImg.Draw(memdc, int(Pibot_x + (653.5 * window_size)), int(Pibot_y + (251 * window_size)), int(50 * window_size), int(50 * window_size), 50 * (int)(Button[3] / 3), 0, 50, 50);
				Button_OrbImg.Draw(memdc, int(Pibot_x + (796.5 * window_size)), int(Pibot_y + (251 * window_size)), int(50 * window_size), int(50 * window_size), 50 * (int)(Button[4] / 3), 50, 50, 50);
				//		수정 필요 GameStatus 2
				if (Time == 0) Button_LampImg.Draw(memdc, int(Pibot_x + 250 * window_size), int(Pibot_y + 550 * window_size), int(120 * window_size), int(120 * window_size), 120 - 120 * PressureCheck(), 0, 120, 120);

				//UI
				
				if (Reactor.cherenkov) DisplayRotatedImage(-860, 0, 20, 110, Reactor.cherenkovmeter / 1500.0 - 1.0 / 3.0, 1);
				else DisplayRotatedImage(-860, 0, 20, 110, Player[0].CherenkovMeter / 1500.0 - 1.0 / 3.0, 1);

				DisplayRotatedImage(-700, -405, 14, 80, PressureCaculate(Mole, Temperture), 2);
				DisplayRotatedImage(-821.5, 300, 80, 80, (Temperture - Kelvin) / (MaxTemp - Kelvin), 3);
				DisplayRotatedImage(-678.5, 300, 80, 80, Mole / MaxPressure * Kelvin / 3 - 1.0 / 6.0, 4);
				DisplayRotatedImage(-825, -355, 15, 190, 0, 5);

				//오브 선택 구버전
				//if (Orbselect_scene) OrbImg.Draw(memdc, int(Pibot_x + 700 * window_size), int(Pibot_y - 380 * window_size), int(100 * window_size), int(100 * window_size), 500, Orb_size * OrbType, Orb_size, Orb_size);
				
				UIScore();
				EffectPrint(EffectHead);

				if (Setting.Debug) UIDebugInfo();

				//if (true) UIDebugInfo();

				if (AnimationTime_Door != 0) {
					DoorImg.Draw(memdc, int(Pibot_x - Controllroom_half_x), int(Pibot_y - Controllroom_half_y), Controllroom_window_x, Controllroom_window_y,
						Controllroom_size_x * ((100 - AnimationTime_Door) / 4 % 5), Controllroom_size_y * (int)((100 - AnimationTime_Door) / 4 / 5),
						Controllroom_size_x, Controllroom_size_y); // 3000 * (PreTime % 5), 2000 * (int)(PreTime / 5)
				}
			}
			else {
				DoorImg.Draw(memdc, int(Pibot_x - Controllroom_half_x), int(Pibot_y - Controllroom_half_y), Controllroom_window_x, Controllroom_window_y,
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
			if (wParam == VK_ESCAPE) Menu_Type = MenuEscape(Menu_Type);
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
				Menu_Type = MenuEscape(Menu_Type);
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
						SelectedButton = 0;
						Time = 0;
						DisplayGame = true;
						Ingame = true;
					}
					break;
				case 2:
					if (SelectedButton == 1 || SelectedButton == 3) Menu_Type = 20 + SelectedButton;
					SelectedButton = 0;
					break;
				case 3:
					if (SelectedButton != 0) Menu_Type = 30 + SelectedButton;
					break;
				case 21:
					if (GetAsyncKeyState(0x10) & 0x8001 || GetAsyncKeyState(0x10) & 0x8000) MassSel *= 5;
					if (GetAsyncKeyState(0x11) & 0x8001 || GetAsyncKeyState(0x11) & 0x8000) MassSel *= 10;

					switch (SelectedButton) {
					case 4:
						CustomRGB[0] = (CustomRGB[1] << 16) | (CustomRGB[2] << 8) | CustomRGB[3];
						PlayerRGB = CustomRGB[0];
						DisplayColorApply(PlayerRGB);
						DisplayReflectorColorApply(PlayerRGB);
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
						if (Player[0].Reflector.module[SelectedModule] == 1) Player[0].Reflector.module[SelectedModule] = 0;
						else Player[0].Reflector.module[SelectedModule] = 1;
					}
					else if (SelectedButton == 3) {
						if (Player[0].Reflector.module[SelectedModule] == 2) Player[0].Reflector.module[SelectedModule] = 0;
						else Player[0].Reflector.module[SelectedModule] = 2;
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

			DoorImg.Draw(memdc, int(Pibot_x - Controllroom_half_x), int(Pibot_y - Controllroom_half_y), Controllroom_window_x, Controllroom_window_y, 0, 0, Controllroom_size_x, Controllroom_size_y);

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
				UIModule(400, 100, true, Player[0].Reflector.module);
				UIModule(400, -100, false, Player[0].Reflector.module);
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

				UIModule(350, 400, SelectedButton == 4, Player[0].Reflector.module);
				UIRGBSlider(CustomRGB[1], CustomRGB[2], CustomRGB[3]);
				UIMenu(SelectedButton, L"Color", L"Red", L"Green", L"Blue", L"Apply", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 23) {
				UIModule(400, 100, true, Player[0].Reflector.module);
				UIModule(400, -100, false, Player[0].Reflector.module);
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
				if (SelectedModule < 5) {
					if (Player[0].Reflector.module[SelectedModule] == 1) {
						rgb[1] = RGBTemplate_Yellow;
						rgb[5] = RGBTemplate_Red;
					}
					else if (Player[0].Reflector.module[SelectedModule] == 2) {
						rgb[3] = RGBTemplate_Yellow;
						rgb[7] = RGBTemplate_Red;
					}
				}
				UIModule(400, 100, true, Player[0].Reflector.module);
				UIModule(400, -100, false, Player[0].Reflector.module);
				if (SelectedModule == 1) UIMenu(SelectedButton, L"Movement", L"Gear", L"", L"Magnet", L"", rgb);
				else if (SelectedModule == 2) UIMenu(SelectedButton, L"Energy", L"Electric", L"", L"Cherenkov", L"", rgb);
				else if (SelectedModule == 3) UIMenu(SelectedButton, L"Reactive", L"Acceleration", L"", L"Decceleration", L"", rgb);
				else if (SelectedModule == 4) UIMenu(SelectedButton, L"Divider", L"Red", L"", L"Blue", L"", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 311) {
				UIMenu(SelectedButton, L"Control", L"Movement", L"Action", L"", L"Reset to defult", rgb);
				UIBack(SelectedButton == -1);
			}
			else if (Menu_Type == 3112) {
				UIMenu(SelectedButton, L"Movement", L"Right", L"Left", L"Up", L"Down", rgb);
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
	return DefWindowProc(hWnd, iMsg, wParam, lParam); //--- 위의 세 메시지 외의 나머지 메시지는 OS로
}




