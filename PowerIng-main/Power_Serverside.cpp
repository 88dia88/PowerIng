#include "Power_ing.h"

///임시로 분리된 코드 - 사용되지 않음

int Power_Serverside(int GameStatus, bool OrbLaunch) {
	Time_Server++;

	if (Reactor.meltdown == false and Orbcount < 0) {
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
		}
	}
}