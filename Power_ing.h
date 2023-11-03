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

extern int Menu;

extern bool WASD, RestartPressure, ChargedMod, debug, keyboard;
extern int PlayerRGB;
extern int Reflector1Left, Reflector1Right, Reflector1Up, Reflector1Down, Player1RGB[3], Player1Charge[3];

extern double window_half, window_size_x, window_size_y, Pibot_x, Pibot_y;
extern double window_size;

extern const int Reactor_size, Rail_size, Orb_size;
extern const int Controllroom_size_x, Reflector_size_x;
extern const int Controllroom_size_y, Reflector_size_y;

extern int Reactor_window, Rail_window, Orb_window;
extern int Controllroom_window_x, Reflector_window_x;
extern int Controllroom_window_y, Reflector_window_y;

extern int Reactor_half, Rail_half, Orb_half;
extern int Controllroom_half_x, Reflector_half_x;
extern int Controllroom_half_y, Reflector_half_y;

extern HDC hdc, memdc;
extern HBITMAP hBitmap;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wparam, LPARAM lparam); // �޼��� ó�� �Լ�
//--------------------------------------------------------------------------------------------------------------//
// 
// Power_Orb
// 
//--------------------------------------------------------------------------------------------------------------//

struct Power_Control {
	int Left, Right, Up, Down;
	int Start;
	bool Button[5];
};

struct Power_Reactor // ������ ����ü
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
	int type, effect, effect_count;
	double speedx, speedy, shellx, shelly;
	double x, y, speed, angle, power, size;
	double afterx[25], aftery[25];
	struct Power_Orb* next;
};
struct Power_Reflector // �г� ����ü
{
	double angle, position, size, speed;
	int module[5], age, effect, rebound;
	bool module_charged[5]; 
	struct Power_Reflector* next;
};
extern bool GameStart;
extern double Score, Temperture, Mole, TotalScore;
extern int Time, PreTime, ReactorEffect, OrbType, Orbcount;
extern int Button[5];
extern int AnimationTime_Door, AnimationTime_Button[5];

extern struct Power_Control Control;
extern struct Power_Reactor Reactor;
extern struct Power_Orb* OrbHead;
extern struct Power_Reflector* ReflectorHead;
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
void ReflectDetect(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // ����Ʈ���� ReflectCheck�� �����ϴ� ���갡 �ִ��� ã�� �Լ�
void ReflectReflector(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // �ݻ��� �� �г��� ��ȭ�� �ٷ�� �Լ�
struct Power_Orb* ReflectReflectorOrb(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // �г��� ���¿� ���� ���� ��ȭ�� �ִ� �Լ�
struct Power_Orb* ReflectOrb(struct Power_Orb* Orb, double Angle); // ���긦 �ݻ��Ű�� �Լ�
//--------------------------------------------------------------------------------------------------------------// �г� ���� �Լ�
void ReflectorPosition(struct Power_Reflector* Reflector, short Left, short Right, short Up, short Down); // �г��� �̵���Ű�� �Լ� 
void ReflectorProcess(struct Power_Reflector* Reflector, bool Reflect); // �г��� �̵���Ű�� �Լ� 

void ReflectorCreate(struct Power_Reflector* Reflector, int Count); // �г��� �߰��ϴ� �Լ�
void ReflectorRemove(struct Power_Reflector* NextReflector, struct Power_Reflector* Reflector); // �г��� �����ϴ� �Լ�
void ReflectClear(); // ��� �г��� �����ϴ� �Լ�
struct Power_Reflector* ReflectorReset(struct Power_Reflector* Reflector); // �г� ���� �ʱ�ȭ�ϴ� �Լ�
struct Power_Reflector* ReflectorApply(struct Power_Reflector* Reflector, int Count); // ���� �г��� ���� �߰� �гο� �����ϴ� �Լ�  
//--------------------------------------------------------------------------------------------------------------//
// 
// Power_Display
// 
//--------------------------------------------------------------------------------------------------------------//
extern CImage ReactorImg, Reactor_EffectImg, ReflectorImg, Reflector_EffectImg, OrbImg, Orb_Animation_Img, PressureImg, CherenkovImg;
extern CImage Reflector_Mask_Img, Reflector_Effect_Mask_Img, Reflector_Color_Mask_Img, Reflector_Light_Mask_Img;
extern CImage Reactor_RailImg, Reflector_ColorImg, Reflector_LightImg, Reflector_ColorChargeImg, Reflector_LightChargeImg, Reflector_ColorOffImg, Reflector_LightOffImg;
extern CImage Button_PressureImg, Button_DialImg, Button_ValveImg, Button_OrbImg, Button_LampImg, Cherenkov_LeverImg, TempertureImg, DoorImg, Door_Light_Img;
extern CImage Pressure_Mask_Img, Cherenkov_Mask_Img, Button_Valve_Mask_Img, Button_Dial_Mask_Img, Temperture_Mask_Img;
extern CImage Reflector_Module_Img, Reflector_Module_Mask_Img;
extern const int RGBTemplate_Red, RGBTemplate_Green, RGBTemplate_Blue, RGBTemplate_Magenta, RGBTemplate_Yellow, RGBTemplate_Cyan, RGBTemplate_White, RGBTemplate_Black, RGBTemplate_Gray;
extern struct Power_Effect* EffectHead; // ����Ʈ�� ����ü
//--------------------------------------------------------------------------------------------------------------//
void CreateEffect(struct Power_Effect* Effect, double x, double y, double Score); // ����Ʈ ��ü ����
void RemoveEffect(struct Power_Effect* Effect, struct Power_Effect* NextEffect); // ����Ʈ ��ü ����
void EffectPrint(struct Power_Effect* Effect); // ����Ʈ ��ü ���
//--------------------------------------------------------------------------------------------------------------//
void DisplayLoad(); // �̹��� �ε� �� ���� ä�� ����
void DisplayColorApply(); // �̹����� ���� ����
void DisplayWindow(); // ������ ũ�⿡ ���� ũ�� ���� ���ϴ� �Լ�
//--------------------------------------------------------------------------------------------------------------//
void DisplayOrb(struct Power_Orb* Orb); // ���� ��� �Լ�
void DisplayReflector(struct Power_Reflector* Reflector); // �г� ��� �Լ�
void DisplayRotatedImage(double x, double y, double Sizex, double Sizey, double Angle, int Type); // ȸ���ϴ� ������Ʈ�� ����ϴ� �Լ�
//--------------------------------------------------------------------------------------------------------------//

int RGBConverter(int RGB, short type);
void UIRGBSlider(int Red, int Green, int Blue);
void UIModule(int x, int y, bool Active);
void UIBack(bool Selected);
void UIMenu(int SelectedButton, const TCHAR String0[30], const TCHAR String1[30], const TCHAR String2[30], const TCHAR String3[30], const TCHAR String4[30], int RGB[9]); // �� �޴� UI
//void UIMenu(bool Start, bool Module, bool Option, bool Quit, bool Esc); // �� �޴� UI
void UIButton(int x, int y, int RGB, bool Seleted, const TCHAR String[30]); // UI ��ư�� ����ϴ� �Լ�
bool UIButtonSelected(int x, int y, int sizex, int sizey, POINTS Mouse); // UI ��ư ���� ���콺�� �ö󰬴��� �Ǵ��ϴ� �Լ�
void UIEndMessage(); // ���� ���� �� ������ ����ϴ� �Լ�
void UIScore(); // ������ ����ϴ� �Լ�
void UIDebugInfo(); // �����÷��� ����׸� ���� ������ ����ϴ� �Լ�
void UIGameStatusDebugInfo(int GameStatus, int EscMode, bool DisplayGame);

//--------------------------------------------------------------------------------------------------------------// �̹��� ȸ�� ��¿� �Լ�
POINT RotatePaint1(double x, double y, double sizex, double sizey, double angle); 
POINT RotatePaint2(double x, double y, double sizex, double sizey, double angle);
POINT RotatePaint3(double x, double y, double sizex, double sizey, double angle);
//-----------------------------------------------------// �г� ȸ�� ��¿� �Լ�
POINT ReflectorPaint1(struct Power_Reflector* Reflector, double Vertical);
POINT ReflectorPaint2(struct Power_Reflector* Reflector, double Vertical);
POINT ReflectorPaint3(struct Power_Reflector* Reflector, double Vertical);
//--------------------------------------------------------------------------------------------------------------//
#endif