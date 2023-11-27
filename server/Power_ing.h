#ifndef _POWER_ORB_
#define _POWER_ORB_
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <tchar.h>
#include <atlImage.h>
#include "Power_Math.h"
#include "resource.h"
//--------------------------------------------------------------------------------------------------------------//
extern HINSTANCE g_hInst;
extern LPCTSTR lpszClass, lpszWindowName;

extern bool Multireflect;

extern int Menu;

extern bool WASD, RestartPressure, ChargedMod, keyboard;
extern int Reflector1Left, Reflector1Right, Reflector1Up, Reflector1Down, Player1Charge[3];

extern int PlayerRGB;

extern double window_half, window_size_x, window_size_y, Pibot_x, Pibot_y;
extern double window_size;

extern const int Reactor_size, Rail_size, Orb_size;
extern const int Controllroom_size_x, Reflector_size_x;
extern const int Controllroom_size_y, Reflector_size_y;

extern int Reactor_window, Rail_window, Orb_window;
extern int Controllroom_window_x, Reflector_window_x;
extern int Controllroom_window_y, Reflector_window_y;

extern const double ModifireValue[6][2];

extern int Reactor_half, Rail_half, Orb_half;
extern int Controllroom_half_x, Reflector_half_x;
extern int Controllroom_half_y, Reflector_half_y;

extern HDC hdc, memdc;
extern HBITMAP hBitmap;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam); // �޼��� ó�� �Լ�
//--------------------------------------------------------------------------------------------------------------//
// 
// Power_Orb
// 
//--------------------------------------------------------------------------------------------------------------//

struct Power_Setting_Player {
	int Control_Active, Control_Left, Control_Up, Control_Right, Control_Down;

	bool Debug;

	bool Game_Cherenkov_auto, Game_PressureReset;
	int Game_ScoreType;
	
	int Display_Resolution, Display_Trail_Quality;

	int Sound_Volume_Master, Sound_Volume, Sound_Alert;
};
struct Power_Setting_Game {
	bool Debug;
	bool GameRule_ModuleCharge;
};
struct Power_Control {
	int Left, Right, Up, Down;
	int Start;
	bool Button[5];
};
struct Power_Reactor // ������ ����ü - ���� ���� ����
{
	bool cherenkov, meltdown;
	int cherenkovlevel, meltdownlevel;
	int cherenkovmeter, cherenkovcounter;
};
struct Power_Effect // �浹 �� ����Ʈ ����ü
{
	double x, y, score;
	int age;
	struct Power_Effect* next;
};
struct Power_Orb // ���� ����ü
{
	bool major;
	double x, y;
	double speed, speedx, speedy, shellx, shelly;
	int RGB;
	int type, effect, effect_count;
	double angle, power, size;
	double afterx[25], aftery[25];
	struct Power_Orb* next;
};
struct Power_Reflector // �г� ����ü
{
	double polar_x, polar_y;
	int polar_speedx, polar_speedy;
	double size, speed;
	int RGB;
	int module[5], age;
	int Effect_effect, Effect_rebound;
	bool module_charged[5];
};
struct Power_Player {
	bool Online, Ready;
	int ID, RGB;
	int Score, CherenkovMeter;
	struct Power_Reflector Reflector;
};

struct Power_Team {
	int RGB;
	int ID[7];
	int life;
};

extern bool GameStart;
extern double Score, Temperture, Mole, TotalScore;
extern int PreTime, ReactorEffect, OrbType, Orbcount;

extern int Time, Time_Server;

extern int Button[5];
extern int AnimationTime_Door, AnimationTime_Button[5];

extern struct Power_Setting_Player Setting;

extern struct Power_Player Player[7];

extern struct Power_Control Control;
extern struct Power_Reactor Reactor;
extern struct Power_Orb* OrbHead;

//--------------------------------------------------------------------------------------------------------------//
bool ReactorMeltdown(); // �� �浹 ȿ�� ����
void ReactorCherenkov();
void GeneralReset(); // �� �ʱ�ȭ �Լ�
void GameRestart(); // ������ �ʱ�ȭ �Ҷ� ���� ���������� �����ϴ� �Լ�
void ButtonActive(); // ��ư �۵� �Լ�
//--------------------------------------------------------------------------------------------------------------//
bool PressureCheck();
//--------------------------------------------------------------------------------------------------------------// ���� ���� �Լ�
struct Power_Orb* OrbPosition(struct Power_Orb* Orb); // ���긦 �̵���Ű�� �ܻ� �迭�� �ִ� �Լ�
struct Power_Orb* OrbSpeed(struct Power_Orb* Orb); // ������ �ӵ��� ���ϴ� �Լ�
void CollisionDetect(struct Power_Orb* Orb); // �浹�� �����ϴ� �Լ�
bool OrbMajorCheck(struct Power_Orb* Orb); // �� �������� Ȯ���ϴ� �Լ�
void OrbCreate(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle); // �� ���긦 �����ϴ� �Լ�
void OrbRemove(struct Power_Orb* NextOrb, struct Power_Orb* Orb); // ���긦 �����ϴ� �Լ�
void OrbClear(); // ��� ���긦 �����ϴ� �Լ�
struct Power_Orb* OrbApply(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle); // ���꿡 �⺻���� �����ϴ� �Լ�
//--------------------------------------------------------------------------------------------------------------// �ݻ� ���� �Լ�
bool ReflectCheck(double x, double y, double angle, double position, double size); // ���갡 �ݻ�Ǳ� �������� �Ǵ��ϴ� �Լ�
struct Power_Reflector ReflectDetect(struct Power_Orb* Orb, struct Power_Reflector Reflector); // ����Ʈ���� ReflectCheck�� �����ϴ� ���갡 �ִ��� ã�� �Լ�
struct Power_Reflector ReflectReflector(struct Power_Orb* Orb, struct Power_Reflector Reflector); // �ݻ��� �� �г��� ��ȭ�� �ٷ�� �Լ�
struct Power_Orb* ReflectReflectorOrb(struct Power_Orb* Orb, struct Power_Reflector Reflector); // �г��� ���¿� ���� ���� ��ȭ�� �ִ� �Լ�
struct Power_Orb* ReflectOrb(struct Power_Orb* Orb, double Angle); // ���긦 �ݻ��Ű�� �Լ�
//--------------------------------------------------------------------------------------------------------------//
struct Power_Reflector ReflectorPosition(struct Power_Reflector Reflector, short Left, short Right, short Up, short Down); // �г��� �̵���Ű�� �Լ� 
struct Power_Reflector ReflectorProcess(struct Power_Reflector Reflector, bool Reflect); // �г��� �̵���Ű�� �Լ� 
struct Power_Reflector ReflectorReset(struct Power_Reflector Reflector); // �г� ���� �ʱ�ȭ�ϴ� �Լ�
//--------------------------------------------------------------------------------------------------------------//�÷��̾� ���� �Լ�
struct Power_Player PlayerReset(struct Power_Player Player, int ID);

struct Power_Setting_Player SettingReset(struct Power_Setting_Player Setting);
int MenuEscape(int Menu_Type);

//--------------------------------------------------------------------------------------------------------------//
#endif