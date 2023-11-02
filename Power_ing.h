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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wparam, LPARAM lparam); // 구버전
BOOL CALLBACK Module_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam); // 구버전
BOOL CALLBACK Option_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam); // 구버전
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

struct Power_Reactor // 리엑터 구조체
{
	bool cherenkov, meltdown;
	int cherenkovlevel, meltdownlevel;
	int cherenkovmeter, cherenkovcounter;
};

struct Power_Effect // 충돌 시 이펙트 구조체
{
	double x, y, score;
	int age;
	struct Power_Effect* next;
};
struct Power_Orb // 오브 구조체
{
	bool major;
	int type, effect, effect_count;
	double speedx, speedy, shellx, shelly;
	double x, y, speed, angle, power, size;
	double afterx[25], aftery[25];
	struct Power_Orb* next;
};
struct Power_Reflector // 패널 구조체
{
	double angle, position, size, speed;
	int module[5], age, effect;
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
bool ReactorMeltdown(); // 공 충돌 효과 판정
void ReactorCherenkov();
void GeneralReset(); // 값 초기화 함수
void GameRestart(); // 게임이 초기화 할때 값을 점진적으로 제거하는 함수
void ButtonActive(); // 버튼 작동 함수
//--------------------------------------------------------------------------------------------------------------//
bool PressureCheck();
//--------------------------------------------------------------------------------------------------------------// 오브 관련 함수
struct Power_Orb* OrbPosition(struct Power_Orb* Orb); // 오브를 이동시키고 잔상에 배열을 넣는 함수
struct Power_Orb* OrbSpeed(struct Power_Orb* Orb); // 오브의 속도를 구하는 함수
void CollisionDetect(struct Power_Orb* Orb); // 충돌을 감지하는 함수
bool OrbMajorCheck(struct Power_Orb* Orb); // 주 오브인지 확인하는 함수
void OrbCreate(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle); // 새 오브를 생성하는 함수
void OrbRemove(struct Power_Orb* NextOrb, struct Power_Orb* Orb); // 오브를 제거하는 함수
void OrbClear(); // 모든 오브를 제거하는 함수
struct Power_Orb* OrbApply(struct Power_Orb* Orb, int Type, bool Major, double x, double y, double Angle); // 오브에 기본값을 적용하는 함수
//--------------------------------------------------------------------------------------------------------------// 반사 관련 함수
bool ReflectCheck(double x, double y, double angle, double position, double size); // 오브가 반사되기 적합한지 판단하는 함수
void ReflectDetect(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // 리스트에서 ReflectCheck를 만족하는 오브가 있는지 찾는 함수
void ReflectReflector(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // 반사할 때 패널의 변화를 다루는 함수
struct Power_Orb* ReflectReflectorOrb(struct Power_Orb* Orb, struct Power_Reflector* Reflector); // 패널의 상태에 따라 공에 변화를 주는 함수
struct Power_Orb* ReflectOrb(struct Power_Orb* Orb, double Angle); // 오브를 반사시키는 함수
//--------------------------------------------------------------------------------------------------------------// 패널 관련 함수
void ReflectorPosition(struct Power_Reflector* Reflector, bool Reflect, short Left, short Right, short Up, short Down); // 패널을 이동시키는 함수 
void ReflectorCreate(struct Power_Reflector* Reflector, int Count); // 패널을 추가하는 함수
void ReflectorRemove(struct Power_Reflector* NextReflector, struct Power_Reflector* Reflector); // 패널을 제거하는 함수
void ReflectClear(); // 모든 패널을 제거하는 함수
struct Power_Reflector* ReflectorReset(struct Power_Reflector* Reflector); // 패널 값을 초기화하는 함수
struct Power_Reflector* ReflectorApply(struct Power_Reflector* Reflector, int Count); // 메인 패널의 값을 추가 패널에 적용하는 함수  
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
extern struct Power_Effect* EffectHead; // 이펙트용 구조체
//--------------------------------------------------------------------------------------------------------------//
void CreateEffect(struct Power_Effect* Effect, double x, double y, double Score); // 이펙트 개체 생성
void RemoveEffect(struct Power_Effect* Effect, struct Power_Effect* NextEffect); // 이펙트 개체 제거
void EffectPrint(struct Power_Effect* Effect); // 이펙트 개체 출력
//--------------------------------------------------------------------------------------------------------------//
void DisplayLoad(); // 이미지 로드 및 알파 채널 적용
void DisplayColorApply(); // 이미지에 색상값 적용
void DisplayWindow(); // 윈도우 크기에 따라 크기 값을 구하는 함수
//--------------------------------------------------------------------------------------------------------------//
void DisplayOrb(struct Power_Orb* Orb); // 오브 출력 함수
void DisplayReflector(struct Power_Reflector* Reflector); // 패널 출력 함수
void DisplayRotatedImage(double x, double y, double Sizex, double Sizey, double Angle, int Type); // 회전하는 오브젝트를 출력하는 함수
//--------------------------------------------------------------------------------------------------------------//
void Menu_UI(int SelectedButton, const TCHAR String0[30], const TCHAR String1[30], const TCHAR String2[30], const TCHAR String3[30], const TCHAR String4[30]); // 신 메뉴 UI


void UIMenu(bool Start, bool Module, bool Option, bool Quit, bool Esc); // 구 메뉴 UI
void UIButton(int x, int y, int R, int G, int B, int SR, int SG, int SB, bool Seleted, const TCHAR String[30]); // UI 버튼을 출력하는 함수
bool UIButtonSelected(int x, int y, int sizex, int sizey, POINTS Mouse); // UI 버튼 위에 마우스가 올라갔는지 판단하는 함수
void UIEndMessage(); // 게임 종료 후 점수를 출력하는 함수
void UIScore(); // 점수를 출력하는 함수
void UIDebugInfo(); // 게임플레이 디버그를 위한 정보를 출력하는 함수
void UIGameStatusDebugInfo(int GameStatus, int EscMode, bool DisplayGame);

//--------------------------------------------------------------------------------------------------------------// 이미지 회전 출력용 함수
POINT RotatePaint1(double x, double y, double sizex, double sizey, double angle); 
POINT RotatePaint2(double x, double y, double sizex, double sizey, double angle);
POINT RotatePaint3(double x, double y, double sizex, double sizey, double angle);
//-----------------------------------------------------// 패널 회전 출력용 함수
POINT ReflectorPaint1(struct Power_Reflector* Reflector, double Vertical);
POINT ReflectorPaint2(struct Power_Reflector* Reflector, double Vertical);
POINT ReflectorPaint3(struct Power_Reflector* Reflector, double Vertical);
//--------------------------------------------------------------------------------------------------------------//
#endif