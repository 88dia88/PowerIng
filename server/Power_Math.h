#pragma once

#ifndef _POWER_MATH_
#define _POWER_MATH_
#define _USE_MATH_DEFINES


#include <math.h>


extern const double M_TU, Kelvin, MaxTemp, MaxMole, MaxPressure;
double PointRotationX(double x, double y, double angle); // 점의 회전 후 x좌표를 계산하는 함수
double PointRotationY(double x, double y, double angle); // 점의 회전 후 x좌표를 계산하는 함수
double PressureCaculate(double Mole, double Temperture); // 압력을 계산하는 함수
double SpeedCaculate(double Power, double Mole, double Temperture); // 속도를 계산하는 함수
double OrbScore(double Speed, double Mole, double Pressure, double Crits, bool Cherenkov); // 점수를 계산하는 함수
double AngleOverflow(double Angle); // 각도의 범위를 0~1로 제한하는 함수
double AnglePosition(double x, double y); // x, y값을 통해 극좌표상의 각(θ)을 찾는 함수
double Reflect(double Angle, double Reflector); // 정반사 각도를 구하는 함수
double DistancePosition(double x, double y); // 중심에서의 거리를 구하기 위해 x,y 좌표를 받는 피타고라스 함수
int PNcmp(double num); //음수 양수 0 판별 함수
bool Distancecmp(double x, double y, double dis); // x,y 좌표값과 거리를 비교해 범위 내부인지 구하는 함수
bool DistanceOvercmp(double x, double y, double dis); // x,y 좌표값과 거리를 비교해 범위 외부인지 구하는 함수
bool DistanceDetect(double x, double y, double Angle, double Distance, double Size); // 공의 위치가 패널과 충돌했는지를 구하기 위한 수식
bool AngleDetect(double x, double y, double Angle); // 공이 패널과 충돌할 만한 극좌표상 각도에 들어와 있는지 구하는 함수
bool ObtuseDetect(double Angle); // 공의 입사각이 둔각인지 구하는 함수
#endif