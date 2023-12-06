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

	static struct Power_Player MainPlayer;

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

		/*
		ReflectorHead->next = ReflectorHead;
		ReflectorHead = ReflectorReset(ReflectorHead);
		*/

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
					EscMode++;
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
						Player[0].Control[1] = GetAsyncKeyState(Reflector1Left);
						Player[0].Control[2] = GetAsyncKeyState(Reflector1Right);
						Player[0].Control[3] = GetAsyncKeyState(Reflector1Up);
						Player[0].Control[4] = GetAsyncKeyState(Reflector1Down);



						/*컨트롤 테스트
						for (int i = 1; i < 7; i++)
						{
							if (Player[i].Online) {
								Player[i].Control[0] = GetAsyncKeyState(Setting.Control_Active);
								Player[i].Control[1] = GetAsyncKeyState(Reflector1Left);
								Player[i].Control[2] = GetAsyncKeyState(Reflector1Right);
								Player[i].Control[3] = GetAsyncKeyState(Reflector1Up);
								Player[i].Control[4] = GetAsyncKeyState(Reflector1Down);
							}
						}
						*/


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