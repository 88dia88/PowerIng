#pragma once

#ifndef _POWER_MATH_
#define _POWER_MATH_
#define _USE_MATH_DEFINES


#include <math.h>


extern const double M_TU, Kelvin, MaxTemp, MaxMole, MaxPressure;
double PointRotationX(double x, double y, double angle); // ���� ȸ�� �� x��ǥ�� ����ϴ� �Լ�
double PointRotationY(double x, double y, double angle); // ���� ȸ�� �� x��ǥ�� ����ϴ� �Լ�
double PressureCaculate(double Mole, double Temperture); // �з��� ����ϴ� �Լ�
double SpeedCaculate(double Power, double Mole, double Temperture); // �ӵ��� ����ϴ� �Լ�
double OrbScore(double Speed, double Mole, double Pressure, double Crits, bool Cherenkov); // ������ ����ϴ� �Լ�
double AngleOverflow(double Angle); // ������ ������ 0~1�� �����ϴ� �Լ�
double AnglePosition(double x, double y); // x, y���� ���� ����ǥ���� ��(��)�� ã�� �Լ�
double Reflect(double Angle, double Reflector); // ���ݻ� ������ ���ϴ� �Լ�
double DistancePosition(double x, double y); // �߽ɿ����� �Ÿ��� ���ϱ� ���� x,y ��ǥ�� �޴� ��Ÿ��� �Լ�
int PNcmp(double num); //���� ��� 0 �Ǻ� �Լ�
bool Distancecmp(double x, double y, double dis); // x,y ��ǥ���� �Ÿ��� ���� ���� �������� ���ϴ� �Լ�
bool DistanceOvercmp(double x, double y, double dis); // x,y ��ǥ���� �Ÿ��� ���� ���� �ܺ����� ���ϴ� �Լ�
bool DistanceDetect(double x, double y, double Angle, double Distance, double Size); // ���� ��ġ�� �гΰ� �浹�ߴ����� ���ϱ� ���� ����
bool AngleDetect(double x, double y, double Angle); // ���� �гΰ� �浹�� ���� ����ǥ�� ������ ���� �ִ��� ���ϴ� �Լ�
bool ObtuseDetect(double Angle); // ���� �Ի簢�� �а����� ���ϴ� �Լ�
#endif