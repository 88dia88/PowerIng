#include "Power_ing.h"
//--------------------------------------------------------------------------------------------------------------//
bool GameStart;
double Score, Temperture, Mole, TotalScore;
int Time, PreTime, ReactorEffect, OrbType, Orbcount;
int Button[5];
int AnimationTime_Door, AnimationTime_Button[5];

struct Power_Setting_Player Setting;

struct Power_Player Player[7];

struct Power_Control Control;
struct Power_Reactor Reactor;
struct Power_Orb* OrbHead = (Power_Orb*)malloc(sizeof(struct Power_Orb));
//--------------------------------------------------------------------------------------------------------------//
bool ReactorMeltdown()
{
	if (Reactor.meltdown) {
		if (Reactor.meltdownlevel < 100) Reactor.meltdownlevel += 4;
		else {
			OrbClear();
			Reactor.meltdown = false;
		}
	}
	else if (Reactor.meltdownlevel > 0) {
		if (Reactor.meltdownlevel == 4) {
			Reactor.meltdownlevel = 0;
			return true;
		}
		Reactor.meltdownlevel -= 4;
	}
	return false;
}
void ReactorCherenkov()
{
	if (Reactor.cherenkov) {
		if (Reactor.cherenkovlevel < 100) Reactor.cherenkovlevel += 2;
		else if (Reactor.cherenkovmeter > 0) Reactor.cherenkovmeter--;
		else Reactor.cherenkov = false;
		if (Reactor.cherenkovcounter > 0) Reactor.cherenkovcounter = 0;
	}
	else {
		if (Reactor.cherenkovlevel > 0) Reactor.cherenkovlevel -= 2;
		if (Reactor.cherenkovcounter > 0)
		{
			if (Reactor.cherenkovmeter < 1000) Reactor.cherenkovmeter++;
			Reactor.cherenkovcounter--;
		}
	}
}
void GeneralReset()
{
	OrbClear();
	//Cherenkov.cherenkov = false, Cherenkov.meter = 0, Cherenkov.counter = 0, Cherenkov.lever = 0;
	Reactor.cherenkov = false, Reactor.cherenkovlevel = 0, Reactor.meltdown = false, Reactor.meltdownlevel = 0, Reactor.cherenkovmeter;
	for (int i = 0; i < 5; i++) Button[i] = 0;
	Time = 0, GameStart = false, Score = 0, ReactorEffect = 0;
	if (RestartPressure) Temperture = Kelvin, Mole = MaxMole * 0.5;
}
void GameRestart()
{
	if (RestartPressure)
	{
		Temperture -= (Temperture - Kelvin) / (31.0 + Time);
		Mole -= (Mole - MaxMole / 2) / (31.0 + Time);
	}
	//Cherenkov.meter -= Cherenkov.meter / (31.0 + Time);
	Reactor.cherenkovmeter -= int(Reactor.cherenkovmeter / (31.0 + Time));
	Score -= Score / (31.0 + Time);
	if (Time % 5 == 0) ReactorEffect--;
	Time--;
}
void ButtonActive()
{
	if ((Button[1] < 0 && Temperture + Button[1] > Kelvin) || (Button[1] > 0 && Temperture + Button[1] < MaxTemp && PressureCaculate(Mole, Temperture + Button[1]) < 1)) Temperture += Button[1];
	else Button[1] = 0;
	if ((Button[2] < 0 && Mole + Button[2] >= 0) || (Button[2] > 0 && Mole + Button[2] < MaxMole && PressureCaculate(Mole + Button[2], Temperture) < 1)) Mole += Button[2];
	else Button[2] = 0;
	if (Button[3] > 0) Button[3]--;
	if (Button[4] > 0) Button[4]--;
}
//--------------------------------------------------------------------------------------------------------------//
bool PressureCheck()
{
	return (PressureCaculate(Mole, Temperture) <= 0.875 && (PressureCaculate(Mole, Temperture) >= 0.375 || PressureCaculate(Mole, Temperture) <= 0));
}
//--------------------------------------------------------------------------------------------------------------//
struct Power_Orb* OrbPosition(struct Power_Orb* Orb)
{
	if (Reactor.cherenkov)
	{
		Orb->x += Orb->speedx * 0.5;
		Orb->y += Orb->speedy * 0.5;
	}
	else
	{
		Orb->x += Orb->speedx;
		Orb->y += Orb->speedy;
	}
	//--------------------------------오브 잔상 매번 배열에 위치값 넣기

	if (Orb->major)
	{
		for (int i = 0; i < Orb->effect_count; i++) {
			Orb->afterx[Orb->effect_count - i] = Orb->afterx[Orb->effect_count - i - 1];
			Orb->aftery[Orb->effect_count - i] = Orb->aftery[Orb->effect_count - i - 1];
		}
		Orb->afterx[0] = Orb->x;
		Orb->aftery[0] = Orb->y;
		if (Orb->effect_count < 24) Orb->effect_count++;
	}
	else
	{
		for (int i = 0; i < Orb->effect_count; i++) {
			Orb->afterx[Orb->effect_count - i] = Orb->afterx[Orb->effect_count - i - 1];
			Orb->aftery[Orb->effect_count - i] = Orb->aftery[Orb->effect_count - i - 1];
		}
		Orb->afterx[0] = Orb->x;
		Orb->aftery[0] = Orb->y;
		if (Orb->effect_count < 24) Orb->effect_count++;
	}
	
	//---------------------------------
	return Orb;
}
struct Power_Orb* OrbSpeed(struct Power_Orb* Orb)
{
	Orb->speed = SpeedCaculate(Orb->power, Mole, Temperture);
	Orb->speedx = Orb->speed * cos(M_TU * Orb->angle) * 5;
	Orb->speedy = Orb->speed * sin(M_TU * Orb->angle) * 5;
	Orb->shellx = cos(M_TU * Orb->angle) * Orb->size;
	Orb->shelly = sin(M_TU * Orb->angle) * Orb->size;
	return Orb;
}
void CollisionDetect(struct Power_Orb* Orb)
{
	if (Orb->next != OrbHead)
	{
		OrbPosition(Orb->next);
		if (DistanceOvercmp(Orb->next->x + Orb->next->shellx, Orb->next->y + Orb->next->shelly, 500))
		{
			if (((Orb->next->major == false && Orb->next->type == 0) || Orb->next->effect == 1) && Distancecmp(Orb->next->x + Orb->next->shellx, Orb->next->y + Orb->next->shelly, 525))
			{
				ReflectOrb(Orb->next, AnglePosition(Orb->next->x, Orb->next->y));
				if (Orb->next->effect == 1) Orb->next->effect = 0;
			}
			else
			{
				if (Orb->next->major)
				{
					Reactor.meltdown = true;

					//팀 목숨 처리

					ReactorEffect = 6;
					if (Orbcount > 0) ReactorEffect = 6;
					else Orbcount = -1;
				}
				OrbRemove(Orb->next, Orb);
			}
		}
		CollisionDetect(Orb->next);
	}
	else return;
}
bool OrbMajorCheck(struct Power_Orb* Orb)
{
	if (Orb->next != OrbHead)
	{
		if (Orb->next->major) return false;
		else OrbMajorCheck(Orb->next);
	}
	else return true;
}
void OrbCreate(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle)
{
	if (Orb->next == OrbHead)
	{
		Orb->next = new Power_Orb;
		Orb->next = OrbApply(Orb->next, Type, Major, x, y, Angle);
		Orb->next->next = OrbHead;
		return;
	}
	else OrbCreate(Orb->next, Type, Major, x, y, Angle);
}
void OrbRemove(struct Power_Orb* NextOrb, struct Power_Orb* Orb)
{
	Orb->next = Orb->next->next;
	free(NextOrb);
	return;
}
void OrbClear()
{
	if (OrbHead->next != OrbHead)
	{
		OrbRemove(OrbHead->next, OrbHead);
		OrbClear();
	}
	else return;
}
struct Power_Orb* OrbApply(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle)
{
	if (Major)
	{
		switch (Type)
		{
		case 1:
			Orb->power = 1.5, Orb->size = 25, Orb->effect = 0;
			break;
		case 2:
			Orb->power = 2, Orb->size = 30, Orb->effect = 0;
			break;
		default:
			Orb->power = 2, Orb->size = 25, Orb->effect = 0;
			break;
		}
	}
	else
	{
		switch (Type)
		{
		case 1:
			Orb->power = 2.5, Orb->size = 18.75, Orb->effect = 0;
			break;
		case 2:
			Orb->power = 2.5, Orb->size = 18.75, Orb->effect = 0;
			break;
		default:
			Orb->power = 2.5, Orb->size = 18.75, Orb->effect = 0;
			break;
		}
	}
	Orb->x = x, Orb->y = y, Orb->type = Type, Orb->major = Major, Orb->angle = Angle, Orb->effect_count = 0;
	Orb = OrbSpeed(Orb);
	return Orb;
}
//--------------------------------------------------------------------------------------------------------------//
bool ReflectCheck(double x, double y, double angle, double position, double size)
{
	return (DistanceOvercmp(x, y, position - 110) &&
		AngleDetect(x, y, angle) &&
		Distancecmp(x, y, position) &&
		DistanceDetect(x, y, AnglePosition(x, y) - angle, position, size));

}
struct Power_Reflector ReflectDetect(struct Power_Orb* Orb, struct Power_Reflector Reflector)
{
	if (Orb->next != OrbHead)
	{
		if (ReflectCheck(Orb->next->x + Orb->next->shellx, Orb->next->y + Orb->next->shelly, Reflector.polar_x, Reflector.polar_y, Reflector.size))
			return ReflectReflector(Orb, Reflector);
		Reflector = ReflectDetect(Orb->next, Reflector);
	}
	else return Reflector;
}
struct Power_Reflector ReflectReflector(struct Power_Orb* Orb, struct Power_Reflector Reflector)
{
	if (Orb->next->major)
	{
		if (Reflector.module_charged[0])
		{
			if (rand() % 3 == 0)
			{
				if (Reflector.module_charged[4])
				{
					OrbCreate(OrbHead, Reflector.module[4], false, Orb->next->x, Orb->next->y, Reflector.polar_x - 0.5);
					Reflector.module_charged[4] = false;
				}
				else OrbCreate(OrbHead, 0, false, Orb->next->x, Orb->next->y, Reflector.polar_x - 0.5);
			}
			ReflectReflectorOrb(Orb->next, Reflector);
			double score = 0, Energy = 1, Cherenks = 1;
			if (Reflector.module_charged[2])
			{
				if (Reflector.module[2] == 2) Cherenks = 1.5;
				else Energy = 1.25;
			}
			switch (Orb->next->type)
			{
			case 1:
				if (Reflector.age > Time - 100) score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 1 * Energy, Reactor.cherenkov);
				else score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 0.5, Reactor.cherenkov);
				if (Reactor.cherenkovmeter < 1000 && Reactor.cherenkov == false) Reactor.cherenkovcounter += int(125 * Cherenks);
				break;
			case 2:
				if (Reflector.age > Time - 100) score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 1.0125 * Energy, Reactor.cherenkov);
				else score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 0.75 * Energy, Reactor.cherenkov);
				if (Reactor.cherenkovmeter < 1000 && Reactor.cherenkov == false) Reactor.cherenkovcounter += int(100 * Orb->next->speed * Orb->next->speed * Cherenks);
				break;
			default:
				if (Reflector.age > Time - 100) score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 1.35 * Energy, Reactor.cherenkov);
				else score = OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 1 * Energy, Reactor.cherenkov);
				if (Reactor.cherenkovmeter < 1000 && Reactor.cherenkov == false) Reactor.cherenkovcounter += int(125 * Cherenks);
				break;
			}
			CreateEffect(EffectHead, Orb->next->x, Orb->next->y, score);
			Score += score;

			Reflector.age = Time + (int)(50 / Orb->next->speed);

			if (score > 1000) Reflector.Effect_effect = Time + 324;
			else if (score > 500) Reflector.Effect_effect = Time + 224;
			else if (score > 250) Reflector.Effect_effect = Time + 124;
			else Reflector.Effect_effect = Time + 24;

			Reflector.Effect_rebound = Time + int(3 * Orb->next->speed);
		}
	}
	else
	{
		switch (Orb->next->type)
		{
		case 0:
			if (Reflector.module[0] != 0 && Reflector.module_charged[0] == false) Reflector.module_charged[0] = true;
			//else if (ReflectorHead->next->module[1] != 0 && ReflectorHead->next->module_charged[1] == false) ReflectorHead->next->module_charged[1] = true;
			else if (Reflector.module[1] != 0 && Reflector.module_charged[1] == false) Reflector.module_charged[1] = true;
			else if (Reflector.module[2] != 0 && Reflector.module_charged[2] == false) Reflector.module_charged[2] = true;
			else if (Reflector.module[3] != 0 && Reflector.module_charged[3] == false) Reflector.module_charged[3] = true;
			else if (Reflector.module[4] != 0 && Reflector.module_charged[4] == false) Reflector.module_charged[4] = true;
			else Score += OrbScore(Orb->next->speed, Mole, PressureCaculate(Mole, Temperture), 1, Reactor.cherenkov);
			break;
		case 1:
			if (Temperture < MaxTemp - 1)	Temperture++;
			else Temperture = MaxTemp;
			break;
		case 2:
			if (Temperture > 1)	Temperture--;
			else Temperture = 0;
			break;
		}
		Reflector.Effect_rebound = Time + int(0.6 * Orb->next->speed);
		OrbRemove(Orb->next, Orb);
	}
	return Reflector;
}
struct Power_Orb* ReflectReflectorOrb(struct Power_Orb* Orb, struct Power_Reflector Reflector)
{
	if (Reflector.module_charged[3])
	{
		if (Reflector.module[3] == 1) Orb->power += 0.1;
		else if (Reflector.module[3] == 2) Orb->power -= 0.1;
	}
	Orb->RGB = Reflector.RGB;
	ReflectOrb(Orb, Reflector.polar_x);
	OrbPosition(Orb);
	return Orb;
}
struct Power_Orb* ReflectOrb(struct Power_Orb* Orb, double Angle)
{
	if (ObtuseDetect(AngleOverflow(Orb->angle - Angle)))
	{
		if (AngleOverflow(Orb->angle - Angle) < 0.5) Orb->angle = AngleOverflow(Orb->angle + 0.25);
		else Orb->angle = AngleOverflow(Orb->angle - 0.25);
	}
	else Orb->angle = Reflect(Orb->angle, Angle);
	OrbSpeed(Orb);
	return Orb;
}
//--------------------------------------------------------------------------------------------------------------//
struct Power_Reflector ReflectorControl(struct Power_Reflector Reflector, short Left, short Right, short Up, short Down)
{
	double Break = 1;
	if ((Up & 0x8001) || (Down & 0x8001) || (Up & 0x8000) || (Down & 0x8000) && Reflector.polar_y < 455 && Reflector.polar_y > 270)
	{
		if (Reflector.module_charged[1])
		{
			switch (Reflector.module[1])
			{
			case 1:
				if ((Up & 0x8000) && Reflector.speed < 2) Reflector.speed += 0.5;
				if ((Down & 0x8000) && Reflector.speed > 0) Reflector.speed -= 0.5;
				break;
			case 2:
				if ((Up & 0x8001) && Reflector.polar_y > 300)  Reflector.polar_y -= 15;
				if ((Down & 0x8001) && Reflector.polar_y < 450) Reflector.polar_y += 15;
				break;
			}
		}
		else
		{
			if (Up & 0x8001) Break = 2;
			if (Down & 0x8001) Break = 0.5;
		}
	}
	else
	{
		if (Reflector.polar_y < 375) Reflector.polar_y += 5;
		else if (Reflector.polar_y > 500) Reflector.polar_y -= (int)((Reflector.polar_y * 0.2 - 75) * 0.2) * 5.0;
		else if (Reflector.polar_y > 375) Reflector.polar_y -= 5;
	}

	Reflector.polar_x = AngleOverflow(Reflector.polar_x + Reflector.polar_speedx * 0.0001 / Reflector.polar_y * 375 * Break);

	if (Right & 0x8001) {
		if (Reflector.polar_speedx > -60)
			Reflector.polar_speedx -= int(Reflector.speed * 10);
	}
	else if (Reflector.polar_speedx < 0) {
		Reflector.polar_speedx += 10;
	}

	if (Left & 0x8001) {
		if (Reflector.polar_speedx < 60)
			Reflector.polar_speedx += int(Reflector.speed * 10);
	}
	else if (Reflector.polar_speedx > 0) {
		Reflector.polar_speedx -= 10;
	}

	return Reflector;
}

struct Power_Reflector ReflectorPosition(struct Power_Reflector Reflector, short Left, short Right, short Up, short Down)
{
	double Break = 1;
	if ((Up & 0x8001) || (Down & 0x8001) || (Up & 0x8000) || (Down & 0x8000) && Reflector.polar_y < 455 && Reflector.polar_y > 270)
	{
		if (Reflector.module_charged[1])
		{
			switch (Reflector.module[1])
			{
			case 1:
				if ((Up & 0x8000) && Reflector.speed < 2) Reflector.speed += 0.5;
				if ((Down & 0x8000) && Reflector.speed > 0) Reflector.speed -= 0.5;
				break;
			case 2:
				if ((Up & 0x8001) && Reflector.polar_y > 300)  Reflector.polar_y -= 15;
				if ((Down & 0x8001) && Reflector.polar_y < 450) Reflector.polar_y += 15;
				break;
			}
		}
		else
		{
			if (Up & 0x8001) Break = 2;
			if (Down & 0x8001) Break = 0.5;
		}
	}
	else
	{
		if (Reflector.polar_y < 375) Reflector.polar_y += 5;
		else if (Reflector.polar_y > 500) Reflector.polar_y -= (int)((Reflector.polar_y * 0.2 - 75) * 0.2) * 5.0;
		else if (Reflector.polar_y > 375) Reflector.polar_y -= 5;
	}

	if ((Right & 0x8001) && (Reflector.polar_speedx > int(-60 * Break))) {
		Reflector.polar_speedx -= int(Reflector.speed * 10);
	}
	else if (Reflector.polar_speedx < 0) {
		Reflector.polar_speedx += 10;
	}

	if ((Left & 0x8001)&& (Reflector.polar_speedx < int(60 * Break))) {
		Reflector.polar_speedx += int(Reflector.speed * 10);
	}
	else if (Reflector.polar_speedx > 0) {
		Reflector.polar_speedx -= 10;
	}

	return Reflector;
}

struct Power_Reflector ReflectorProcess(struct Power_Reflector Reflector, bool Reflect)
{
	if (Reflect && Reflector.age <= Time)
		Reflector = ReflectDetect(OrbHead, Reflector);

	Reflector.polar_x = AngleOverflow(Reflector.polar_x + Reflector.polar_speedx * 0.0001 / Reflector.polar_y * 375);

	if (Reflector.age <= Time -101 || Time < 1) Reflector.age = 0;

	if (Reflector.Effect_effect <= Time || Time < 1 || (Reflector.Effect_effect - Time) % 100 == 0) Reflector.Effect_effect = 0;
	if (Reflector.Effect_rebound <= Time || Time < 1) Reflector.Effect_rebound = 0;

	return Reflector;
}
struct Power_Reflector ReflectorReset(struct Power_Reflector Reflector)
{
	Reflector.module[0] = 0, Reflector.module[1] = 0, Reflector.module[2] = 0, Reflector.module[3] = 0, Reflector.module[4] = 0;
	for (int i = 0; i < 5; i++)
	{
		if (Reflector.module[i] != 0) Reflector.module_charged[i] = ChargedMod;
		else Reflector.module_charged[i] = false;
	}
	Reflector.module_charged[0] = true;
	Reflector.polar_y = 375;
	if (Reflector.module[0] & 1) Reflector.polar_x = 0;
	else Reflector.polar_x = 0.25;
	Reflector.size = 375, Reflector.speed = 1, Reflector.age = 0;
	Reflector.Effect_effect = 0, Reflector.Effect_rebound = 0;
	return Reflector;
}
//--------------------------------------------------------------------------------------------------------------//
struct Power_Player PlayerReset(struct Power_Player Player, int ID) {
	Player.Online = true;
	Player.ID = ID, Player.RGB = RGBTemplate_Yellow;
	Player.Score = 0, Player.CherenkovMeter = 0;
	Player.Ready = false;
	Player.Reflector = ReflectorReset(Player.Reflector);
	return Player;
}

struct Power_Setting_Player SettingReset(struct Power_Setting_Player Setting) {
	Setting.Control_Active = 0x0D;
	Setting.Control_Left = 0x25;
	Setting.Control_Up = 0X26;
	Setting.Control_Right = 0X27;
	Setting.Control_Down = 0X28;

	Setting.Debug = false;

	Setting.Game_Cherenkov_auto = false;
	Setting.Game_PressureReset = false;
	Setting.Game_ScoreType = 0;

	Setting.Display_Resolution = 0.8;
	Setting.Display_Trail_Quality = 0;

	Setting.Sound_Volume_Master = 100;
	Setting.Sound_Volume = 100;
	Setting.Sound_Alert = 0;
	
	return Setting;
}

int MenuEscape(int Menu_Type) {
	if (Menu_Type / 10 == 2) Menu_Type = 2;
	else if (Menu_Type / 10 == 3) Menu_Type = 3;
	else if (Menu_Type / 10 == 23) Menu_Type = 23;
	else if (Menu_Type / 10 == 31) Menu_Type = 31;
	else Menu_Type = 0;
	return Menu_Type;
}

