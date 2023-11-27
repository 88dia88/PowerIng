#include "Power_Math.h"
const double M_TU = M_PI * 2, Kelvin = 273.15, MaxTemp = 1000, MaxMole = 500, MaxPressure = 100000;
double PointRotationX(double x, double y, double angle)
{
	return x * cos(M_TU * angle) - y * sin(M_TU * angle);
}
double PointRotationY(double x, double y, double angle)
{
	return x * sin(M_TU * angle) + y * cos(M_TU * angle);
}
double PressureCaculate(double Mole, double Temperture)
{
	return Mole * Temperture / MaxPressure;
}
double SpeedCaculate(double Power, double Mole, double Temperture)
{
	return Power * (1 - (Mole / MaxMole));
}
double OrbScore(double Speed, double Mole, double Pressure, double Crits, bool Cherenkov)
{
	Speed = Speed * Speed * 100;
	if (Mole <= 0) return Speed * 0.75 * Crits;
	else if (Pressure < 0.5) return (Speed * (Pressure + 0.25) + (Speed * Pressure * Cherenkov)) * Crits;
	else return (Speed + (Speed * Cherenkov)) * Crits;
}
double AngleOverflow(double Angle)
{
	if (Angle >= 1)
		Angle--;
	else if (Angle < 0)
		Angle++;
	return Angle;
}
double AnglePosition(double x, double y)
{
	return atan2( - y, - x) / M_TU + 0.5;
}
double Reflect(double Angle, double Reflector)
{
	Angle = 0.5 - Angle + Reflector * 2;
	return Angle;
}
double DistancePosition(double x, double y)
{
	return sqrt((x * x) + (y * y));
}
int PNcmp(double num)
{
	if (num > 0) return 1;
	else if (num < 0) return -1;
	else return 0;
}
bool Distancecmp(double x, double y, double dis)
{
	return ((x * x) + (y * y) < dis * dis);
}
bool DistanceOvercmp(double x, double y, double dis)
{
	return ((x * x) + (y * y) > dis * dis);
}
bool DistanceDetect(double x, double y, double Angle, double Distance, double Size)
{
	return (x * x + y * y > (Distance * Distance - Size * Size * 0.25) / (cos(Angle * M_TU) * cos(Angle * M_TU)));
}
bool AngleDetect(double x, double y, double Angle)
{	
	Angle = AngleOverflow(AnglePosition(x, y) - Angle);
	return (Angle < 0.13) || (0.87 < Angle);
}
bool ObtuseDetect(double Angle)
{
	AngleOverflow(Angle);
	return ((0.125 < Angle && Angle <= 0.25) || (0.75 <= Angle && Angle < 0.875));
}