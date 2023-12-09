#include "Power_ing.h"

///임시로 분리된 코드 - 사용되지 않음

int Power_Serverside(int GameStatus, bool OrbLaunch) {
	Time_Server++;

	if (Reactor.meltdown == false and Player[0].Count <= 0) {
		Player[0].Count--;
		GameStatus = -2;
		//게임오버
	}
	else
	{
		if (ReactorMeltdown() and GameStatus == 1) return 2;
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
}