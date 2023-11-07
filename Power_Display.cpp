#include "Power_ing.h"
//--------------------------------------------------------------------------------------------------------------//
double window_half = window_size * 0.5;
double window_size_x = 2000 * window_size, window_size_y = 1125 * window_size, Pibot_x = window_size_x * 0.5, Pibot_y = window_size_y * 0.5;

const int Reactor_size = 1000, Rail_size = 375, Orb_size = 50;
const int Controllroom_size_x = 3000, Reflector_size_x = 375;
const int Controllroom_size_y = 2000, Reflector_size_y = 115;

int Reactor_window = int(Reactor_size * window_size), Rail_window = int(Rail_size * window_size), Orb_window = int(Orb_size * window_size);
int Controllroom_window_x = int(Controllroom_size_x * window_size), Reflector_window_x = int(Reflector_size_x * window_size);
int Controllroom_window_y = int(Controllroom_size_y * window_size), Reflector_window_y = int(Reflector_size_y * window_size);

int Reactor_half = int(Reactor_size * window_half), Rail_half = int(Rail_size * window_half), Orb_half = int(Orb_size * window_half);
int Controllroom_half_x = int(Controllroom_size_x * window_half), Reflector_half_x = int(Reflector_size_x * window_half);
int Controllroom_half_y = int(Controllroom_size_y * window_half), Reflector_half_y = int(Reflector_size_y * window_half);

const int RGBTemplate_Red = 0xff0000, RGBTemplate_Green = 0x00ff00, RGBTemplate_Blue = 0x0000ff,
	RGBTemplate_Magenta = 0xff00ff, RGBTemplate_Yellow = 0xffff00, RGBTemplate_Cyan = 0x00ffff,
	RGBTemplate_White = 0xffffff, RGBTemplate_Black = 0x000000, RGBTemplate_Gray = 0x808080;

//--------------------------------------------------------------------------------------------------------------//
CImage ReactorImg, Reactor_EffectImg, ReflectorImg, Reflector_EffectImg, OrbImg, Orb_Animation_Img, PressureImg, CherenkovImg;
CImage Reactor_RailImg, Reflector_ColorImg, Reflector_LightImg, Reflector_ColorChargeImg, Reflector_LightChargeImg, Reflector_ColorOffImg, Reflector_LightOffImg;
CImage Reflector_Mask_Img, Reflector_Effect_Mask_Img, Reflector_Color_Mask_Img, Reflector_Light_Mask_Img;
CImage Button_PressureImg, Button_DialImg, Button_ValveImg, Button_OrbImg, Button_LampImg, Cherenkov_LeverImg, TempertureImg, DoorImg, Door_Light_Img;
CImage Pressure_Mask_Img, Cherenkov_Mask_Img, Button_Valve_Mask_Img, Button_Dial_Mask_Img, Temperture_Mask_Img;
CImage Reflector_Module1_Img, Reflector_Module2_Img, Reflector_Module3_Img, Reflector_Module4_Img, Reflector_Module5_Img, Reflector_Module_Img, Reflector_Module_Mask_Img;
CImage Reflector_Module1_Mask_Img, Reflector_Module2_Mask_Img, Reflector_Module3_Mask_Img, Reflector_Module4_Mask_Img, Reflector_Module5_Mask_Img;
struct Power_Effect* EffectHead = (Power_Effect*)malloc(sizeof(struct Power_Effect));
//--------------------------------------------------------------------------------------------------------------//
void CreateEffect(struct Power_Effect* Effect, double x, double y, double Score)
{
	if (Effect->next == EffectHead)
	{
		Effect->next = new Power_Effect;
		Effect->next->x = x;
		Effect->next->y = y;
		Effect->next->score = Score;
		Effect->next->age = Time + 100;
		Effect->next->next = EffectHead;
		return;
	}
	else CreateEffect(Effect->next, x, y, Score);
}
void RemoveEffect(struct Power_Effect* NextEffect, struct Power_Effect* Effect)
{
	Effect->next = Effect->next->next;
	free(NextEffect);
	return;
}
void EffectPrint(struct Power_Effect* Effect)
{
	if (Effect->next != EffectHead)
	{
		if (Effect->next->age == Time || Time < 1)
		{
			RemoveEffect(Effect->next, Effect);
		}
		else
		{
			TCHAR lpOut[100];
			HFONT hFont, oldFont;
			int Age = 100 - Effect->next->age + Time;
			hFont = CreateFont((int)(100 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
			oldFont = (HFONT)SelectObject(memdc, hFont);
			if (Effect->next->score > 1000)
				SetTextColor(memdc, RGB(255, 0, 255));
			else if (Effect->next->score > 500)
				SetTextColor(memdc, RGB(0, 255, 255));
			else if (Effect->next->score > 250)
				SetTextColor(memdc, RGB(255, 255, 0));
			else
				SetTextColor(memdc, RGB(255, 255, 255));
			if (Effect->next->score > 0) swprintf_s(lpOut, 100, L"+\\%.0f ", Effect->next->score);
			else swprintf_s(lpOut, 100, L"-\\%.0f ", Effect->next->score * -1);
			TextOut(memdc, (int)(Pibot_x + Effect->next->x * window_size), (int)(Pibot_y + (Effect->next->y - Age * 2.0) * window_size), lpOut, lstrlen(lpOut));
			SelectObject(memdc, oldFont);
			DeleteObject(hFont);
		}
		EffectPrint(Effect->next);
	}
	else return;
}
//--------------------------------------------------------------------------------------------------------------//
void DisplayLoad()
{
	ReactorImg.Load(TEXT("Img\\Reactor.png"));
	Reactor_EffectImg.Load(TEXT("Img\\Reactor_Effect.png"));
	OrbImg.Load(TEXT("Img\\Orb.png"));
	Orb_Animation_Img.Load(TEXT("Img\\Orb_Animation.png"));
	PressureImg.Load(TEXT("Img\\Pressure.png"));
	CherenkovImg.Load(TEXT("Img\\Cherenkov.png"));

	ReflectorImg.Load(TEXT("Img\\Reflector.png"));
	Reflector_EffectImg.Load(TEXT("Img\\Reflector_Effect.png"));

	Reflector_Mask_Img.Load(TEXT("Img\\Reflector_Mask.bmp"));
	Reflector_Effect_Mask_Img.Load(TEXT("Img\\Reflector_Effect_Mask.bmp"));
	Reflector_Color_Mask_Img.Load(TEXT("Img\\Reflector_Color_Mask.bmp"));
	Reflector_Light_Mask_Img.Load(TEXT("Img\\Reflector_Light_Mask.bmp"));

	Button_PressureImg.Load(TEXT("Img\\Button_Pressure.png"));
	Button_DialImg.Load(TEXT("Img\\Button_Dial.png"));
	Button_ValveImg.Load(TEXT("Img\\Button_Valve.png"));
	Button_OrbImg.Load(TEXT("Img\\Button_Orb.png"));
	Button_LampImg.Load(TEXT("Img\\Button_Lamp.png"));
	Cherenkov_LeverImg.Load(TEXT("Img\\Cherenkov_Lever.png"));
	TempertureImg.Load(TEXT("Img\\Temperture.png"));
	DoorImg.Load(TEXT("Img\\Door.png"));
	Door_Light_Img.Load(TEXT("Img\\Door_Light.png"));

	Pressure_Mask_Img.Load(TEXT("Img\\Pressure_Mask.bmp"));
	Cherenkov_Mask_Img.Load(TEXT("Img\\Cherenkov_Mask.bmp"));
	Button_Dial_Mask_Img.Load(TEXT("Img\\Button_Dial_Mask.bmp"));
	Button_Valve_Mask_Img.Load(TEXT("Img\\Button_Valve_Mask.bmp"));
	Temperture_Mask_Img.Load(TEXT("Img\\Temperture_Mask.bmp"));

	Reflector_Module_Img.Load(TEXT("Img\\Reflector_Module.png"));
	Reflector_Module_Mask_Img.Load(TEXT("Img\\Reflector_Module_Mask.bmp"));

	for (int i = 0; i < Door_Light_Img.GetWidth(); i++)
	{
		for (int j = 0; j < Door_Light_Img.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Door_Light_Img.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Reactor_EffectImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reactor_EffectImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reactor_EffectImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Cherenkov_LeverImg.GetWidth(); i++)
	{
		for (int j = 0; j < Cherenkov_LeverImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Cherenkov_LeverImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Button_PressureImg.GetWidth(); i++)
	{
		for (int j = 0; j < Button_PressureImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Button_PressureImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Button_OrbImg.GetWidth(); i++)
	{
		for (int j = 0; j < Button_OrbImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Button_OrbImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Button_LampImg.GetWidth(); i++)
	{
		for (int j = 0; j < Button_LampImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Button_LampImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < OrbImg.GetWidth(); i++)
	{
		for (int j = 0; j < OrbImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)OrbImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Orb_Animation_Img.GetWidth(); i++)
	{
		for (int j = 0; j < Orb_Animation_Img.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Orb_Animation_Img.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < ReflectorImg.GetWidth(); i++)
	{
		for (int j = 0; j < ReflectorImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)ReflectorImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
	for (int i = 0; i < Reflector_Module_Img.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_Module_Img.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_Module_Img.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
			ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
			ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
		}
	}
}
void DisplayWindow()
{
	window_half = window_size * 0.5;
	window_size_x = 2000 * window_size, window_size_y = 1125 * window_size, Pibot_x = window_size_x * 0.5, Pibot_y = window_size_y * 0.5;

	Reactor_window = int(Reactor_size * window_size), Rail_window = int(Rail_size * window_size), Orb_window = int(Orb_size * window_size);
	Controllroom_window_x = int(Controllroom_size_x * window_size), Reflector_window_x = int(Reflector_size_x * window_size);
	Controllroom_window_y = int(Controllroom_size_y * window_size), Reflector_window_y = int(Reflector_size_y * window_size);

	Reactor_half = int(Reactor_size * window_half), Rail_half = int(Rail_size * window_half), Orb_half = int(Orb_size * window_half);
	Controllroom_half_x = int(Controllroom_size_x * window_half), Reflector_half_x = int(Reflector_size_x * window_half);
	Controllroom_half_y = int(Controllroom_size_y * window_half), Reflector_half_y = int(Reflector_size_y * window_half);
}
void DisplayColorApply(int RGB)
{
	if (!Reactor_RailImg.IsNull())
		Reactor_RailImg.Destroy();
	if (!Reflector_ColorImg.IsNull())
		Reflector_ColorImg.Destroy();
	if (!Reflector_LightImg.IsNull())
		Reflector_LightImg.Destroy();
	if (!Reflector_ColorChargeImg.IsNull())
		Reflector_ColorChargeImg.Destroy();
	if (!Reflector_LightChargeImg.IsNull())
		Reflector_LightChargeImg.Destroy();
	if (!Reflector_ColorOffImg.IsNull())
		Reflector_ColorOffImg.Destroy();
	if (!Reflector_LightOffImg.IsNull())
		Reflector_LightOffImg.Destroy();
	Reactor_RailImg.Load(TEXT("Img\\Reactor_Rail.png"));
	Reflector_ColorImg.Load(TEXT("Img\\Reflector_Color.png"));
	Reflector_LightImg.Load(TEXT("Img\\Reflector_Color.png"));
	Reflector_ColorChargeImg.Load(TEXT("Img\\Reflector_Color.png"));
	Reflector_LightChargeImg.Load(TEXT("Img\\Reflector_Color.png"));
	Reflector_ColorOffImg.Load(TEXT("Img\\Reflector_Color.png"));
	Reflector_LightOffImg.Load(TEXT("Img\\Reflector_Color.png"));

	int rgb[4] = { 0, 0, 0, 0 };
	for (int i = 1; i < 4; i++)
	{
		rgb[i] = RGBConverter(RGB, i);
	}
	for (int i = 0; i < Reactor_RailImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reactor_RailImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reactor_RailImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] - 255 + rgb[3]) * ptr[3] + 127) / 255;
			ptr[1] = ((ptr[1] - 255 + rgb[2]) * ptr[3] + 127) / 255;
			ptr[2] = ((ptr[2] - 255 + rgb[1]) * ptr[3] + 127) / 255;
		}
	}
	for (int i = 0; i < Reflector_LightImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_LightImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_LightImg.GetPixelAddress(i, j);
			ptr[0] = ((ptr[0] - 255 + rgb[3]) * ptr[3] + 127) / 255;
			ptr[1] = ((ptr[1] - 255 + rgb[2]) * ptr[3] + 127) / 255;
			ptr[2] = ((ptr[2] - 255 + rgb[1]) * ptr[3] + 127) / 255;
		}
	}
	for (int i = 0; i < Reflector_ColorImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_ColorImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_ColorImg.GetPixelAddress(i, j);
			ptr[0] = BYTE(((ptr[0] - 255 + rgb[3] * 0.9) * ptr[3] + 127) / 255);
			ptr[1] = BYTE(((ptr[1] - 255 + rgb[2] * 0.9) * ptr[3] + 127) / 255);
			ptr[2] = BYTE(((ptr[2] - 255 + rgb[1] * 0.9) * ptr[3] + 127) / 255);
		}
	}
	for (int i = 0; i < Reflector_LightChargeImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_LightChargeImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_LightChargeImg.GetPixelAddress(i, j);
			ptr[0] = BYTE(((ptr[0] - 26 + rgb[3] * 0.1) * ptr[3] + 127) / 255);
			ptr[1] = BYTE(((ptr[1] - 26 + rgb[2] * 0.1) * ptr[3] + 127) / 255);
			ptr[2] = BYTE(((ptr[2] - 26 + rgb[1] * 0.1) * ptr[3] + 127) / 255);
		}
	}
	for (int i = 0; i < Reflector_ColorChargeImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_ColorChargeImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_ColorChargeImg.GetPixelAddress(i, j);
			ptr[0] = BYTE(((ptr[0] - 255 + rgb[3]) * ptr[3] + 127) / 255);
			ptr[1] = BYTE(((ptr[1] - 255 + rgb[2]) * ptr[3] + 127) / 255);
			ptr[2] = BYTE(((ptr[2] - 255 + rgb[1]) * ptr[3] + 127) / 255);
		}
	}
	for (int i = 0; i < Reflector_LightOffImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_LightOffImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_LightOffImg.GetPixelAddress(i, j);
			ptr[0] = BYTE(((ptr[0] - 196 + rgb[3] * 0.5) * ptr[3] + 127) / 255);
			ptr[1] = BYTE(((ptr[1] - 196 + rgb[2] * 0.5) * ptr[3] + 127) / 255);
			ptr[2] = BYTE(((ptr[2] - 196 + rgb[1] * 0.5) * ptr[3] + 127) / 255);
		}
	}
	for (int i = 0; i < Reflector_ColorOffImg.GetWidth(); i++)
	{
		for (int j = 0; j < Reflector_ColorOffImg.GetHeight(); j++)
		{
			BYTE* ptr = (BYTE*)Reflector_ColorOffImg.GetPixelAddress(i, j);
			ptr[0] = BYTE(((ptr[0] - 196 + rgb[3] * 0.35) * ptr[3] + 127) / 255);
			ptr[1] = BYTE(((ptr[1] - 196 + rgb[2] * 0.35) * ptr[3] + 127) / 255);
			ptr[2] = BYTE(((ptr[2] - 196 + rgb[1] * 0.35) * ptr[3] + 127) / 255);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------//
void DisplayOrb(struct Power_Orb* Orb)
{
	if (Orb->next != OrbHead)
	{
		for (int j = 2; j < 6; j++) 
			for (int i = Orb->next->effect_count; i > 0; i--) 
				OrbImg.AlphaBlend(memdc, 
					int(Pibot_x + (Orb->next->afterx[i] - Orb->next->size) * window_size), 
					int(Pibot_y + (Orb->next->aftery[i] - Orb->next->size) * window_size), 
					int(Orb->next->size * 2 * window_size), 
					int(Orb->next->size * 2 * window_size), 
					Orb_size * (4 * Reactor.cherenkov + 10 * Orb->next->major + j), 
					Orb_size * Orb->next->type, 
					Orb_size, Orb_size, 
					BYTE(1.5 * (Orb->next->effect_count - i)));

		OrbImg.Draw(memdc, int(Pibot_x + (Orb->next->x - Orb->next->size) * window_size), int(Pibot_y + (Orb->next->y - Orb->next->size) * window_size), int(Orb->next->size * 2 * window_size), int(Orb->next->size * 2 * window_size), Orb_size * (Reactor.cherenkov + 10 * Orb->next->major), Orb_size * Orb->next->type, Orb_size, Orb_size);
		DisplayOrb(Orb->next);
	}
	else return;
}
void DisplayReflector(struct Power_Reflector_Pointless Reflector)
{
	POINT Reflector_Point[3] = {
		ReflectorPaint1(Reflector, 0),
		ReflectorPaint2(Reflector, 0),
		ReflectorPaint3(Reflector, 0)
	};
	ReflectorImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Mask_Img, 0, 0);

	if (Reflector.age < -100 && Reflector.module_charged[0])
	{
		Reflector_ColorImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Color_Mask_Img, 0, 0);
		Reflector_LightImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Light_Mask_Img, 0, 0);
	}
	else if (Reflector.age < 0 && Reflector.module_charged[0])
	{
		Reflector_ColorChargeImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Color_Mask_Img, 0, 0);
		Reflector_LightChargeImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Light_Mask_Img, 0, 0);
	}
	else
	{
		Reflector_ColorOffImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Color_Mask_Img, 0, 0);
		Reflector_LightOffImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Light_Mask_Img, 0, 0);
	}

	Reflector_Module_Img.PlgBlt(memdc, Reflector_Point, 375 * Reflector.module_charged[4] + 750 * 4, (115 * (Reflector.module[4] - 1)), 375, 115, Reflector_Module_Mask_Img, 375 * Reflector.module_charged[4] + 750 * 4, (115 * (Reflector.module[4] - 1)));
	for (int i = 0; i < 4; i++)
	{
		Reflector_Module_Img.PlgBlt(memdc, Reflector_Point, 375 * Reflector.module_charged[i] + 750 * i, (115 * (Reflector.module[i] - 1)), 375, 115, Reflector_Module_Mask_Img, 375 * Reflector.module_charged[i] + 750 * i, (115 * (Reflector.module[i] - 1)));
	}

	if (Reflector.effect > 0)
	{
		POINT Reflector_Effect_Point[3] = {
		ReflectorPaint1(Reflector, 67.5),
		ReflectorPaint2(Reflector, 67.5),
		ReflectorPaint3(Reflector, 67.5)
		};
		Reflector_EffectImg.PlgBlt(memdc, Reflector_Effect_Point, 375 * (Reflector.effect % 100), (300 * (int)(Reflector.effect / 100.0)), 375, 300, Reflector_Effect_Mask_Img, 375 * (Reflector.effect % 100), 0);
	}
	return;
}
void DisplayRotatedImage(double x, double y, double Sizex, double Sizey, double Angle, int Type)
{
	POINT Image_Point[3] = {
	RotatePaint1(x, y, Sizex, Sizey, Angle),
	RotatePaint2(x, y, Sizex, Sizey, Angle),
	RotatePaint3(x, y, Sizex, Sizey, Angle)
	};
	switch (Type)
	{
	case 1:
		CherenkovImg.PlgBlt(memdc, Image_Point, 0, 0, int(Sizex), int(Sizey), Cherenkov_Mask_Img, 0, 0);
		break;
	case 2:
		PressureImg.PlgBlt(memdc, Image_Point, 0, 0, int(Sizex), int(Sizey), Pressure_Mask_Img, 0, 0);
		break;
	case 3:
		Button_DialImg.PlgBlt(memdc, Image_Point, 0, 0, int(Sizex), int(Sizey), Button_Dial_Mask_Img, 0, 0);
		break;
	case 4:
		Button_ValveImg.PlgBlt(memdc, Image_Point, 0, 0, int(Sizex), int(Sizey), Button_Valve_Mask_Img, 0, 0);
		break;
	case 5:
		TempertureImg.PlgBlt(memdc, Image_Point, 0, 0, int(Sizex), int(Sizey), Temperture_Mask_Img, 0, int(Temperture / MaxTemp * 190));
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------------------------------------------------//
int RGBConverter(int RGB, short type) {
	int rgb;
	switch (type)
	{
	case 1:
		rgb = RGB & 0xff0000;
		return (rgb >> 16);
		break;
	case 2:
		rgb = RGB & 0x00ff00;
		return (rgb >> 8);
		break;
	case 3:
		rgb = RGB & 0x0000ff;
		return (rgb >> 0);
		break;
	default:
		return 0;
		break;
	}
}
void UIRGBSlider(int Red, int Green, int Blue) {
	HFONT hFont, oldFont;
	hFont = CreateFont((int)(150 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);
	TCHAR lpOut[100];

	SetTextColor(memdc, RGB(255, 0, 0));
	TextOut(memdc, int(Pibot_x + 200 * window_size), int(Pibot_y - 125 * window_size), L"<|          |>", lstrlen(L"<|          |>"));
	swprintf_s(lpOut, 100, L"%03d", Red);
	TextOut(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y - 125 * window_size), lpOut, lstrlen(lpOut));
	SetTextColor(memdc, RGB(0, 255, 0));
	TextOut(memdc, int(Pibot_x + 200 * window_size), int(Pibot_y + 25 * window_size), L"<|          |>", lstrlen(L"<|          |>"));
	swprintf_s(lpOut, 100, L"%03d", Green);
	TextOut(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y + 25 * window_size), lpOut, lstrlen(lpOut));
	SetTextColor(memdc, RGB(0, 0, 255));
	TextOut(memdc, int(Pibot_x + 200 * window_size), int(Pibot_y + 175 * window_size), L"<|          |>", lstrlen(L"<|          |>"));
	swprintf_s(lpOut, 100, L"%03d", Blue);
	TextOut(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y + 175 * window_size), lpOut, lstrlen(lpOut));
}

void UIModule(int x, int y, bool Active, int module[5]) {
	ReflectorImg.AlphaBlend(memdc, int(Pibot_x + x * window_size), int(Pibot_y + (y - 57.5) * window_size), int(375 * window_size), int(115 * window_size), 0, 0, 375, 115, 255);
	if (Active) {
		Reflector_ColorImg.AlphaBlend(memdc, int(Pibot_x + x * window_size), int(Pibot_y + (y - 57.5) * window_size), int(375 * window_size), int(115 * window_size), 0, 0, 375, 115, 255);
		/*
		POINT Reflector_Point[3] = {
			int(Pibot_x + x * window_size),int(Pibot_y + (y + 57.5) * window_size),
			int(Pibot_x + (x + 375) * window_size),int(Pibot_y + (y + 57.5) * window_size),
			int(Pibot_x + x * window_size),int(Pibot_y + (y - 57.5) * window_size)
		};
		Reflector_LightImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Light_Mask_Img, 0, 0);
		*/
	}		
	else {
		Reflector_ColorOffImg.AlphaBlend(memdc, int(Pibot_x + x * window_size), int(Pibot_y + (y - 57.5) * window_size), int(375 * window_size), int(115 * window_size), 0, 0, 375, 115, 255);
		/*
		POINT Reflector_Point[3] = {
			int(Pibot_x + x * window_size),int(Pibot_y + (y + 57.5) * window_size),
			int(Pibot_x + (x + 375) * window_size),int(Pibot_y + (y + 57.5) * window_size),
			int(Pibot_x + x * window_size),int(Pibot_y + (y - 57.5) * window_size)
		};
		Reflector_LightOffImg.PlgBlt(memdc, Reflector_Point, 0, 0, 375, 115, Reflector_Light_Mask_Img, 0, 0);
		*/
	}
	


	for (int i = 1; i < 5; i++)
	{
		Reflector_Module_Img.AlphaBlend(memdc, int(Pibot_x + x * window_size), int(Pibot_y + (y - 57.5) * window_size), int(375 * window_size), int(115 * window_size), 375 * Active + 750 * i, 115 * (module[i] - 1), 375, 115, 255);
	}
	/*
	ReflectorImg.Draw(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y * window_size), int(375 * window_size), int(115 * window_size), 0, 0, 375, 115);
	Reflector_ColorImg.Draw(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y * window_size), int(375 * window_size), int(115 * window_size), 0, 0, 375, 115);
	for (int i = 1; i < 5; i++)
	{
		Reflector_Module_Img.Draw(memdc, int(Pibot_x + 400 * window_size), int(Pibot_y * window_size), int(375 * window_size), int(115 * window_size), 375 + 750 * i, 115 * (ReflectorHead->module[i] - 1), 375, 115);
	}
	*/
}
void UIBack(bool Selected)
{
	HFONT hFont, oldFont;
	hFont = CreateFont((int)(150 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);

	if (Selected) {
		SetTextColor(memdc, RGB(255, 0, 0));
		TextOut(memdc, int(Pibot_x + 200 * window_size), int(Pibot_y - 500 * window_size), L"<= back", lstrlen(L"<= back"));
	}
	else {
		SetTextColor(memdc, RGB(255, 255, 0));
		TextOut(memdc, int(Pibot_x + 200 * window_size), int(Pibot_y - 500 * window_size), L"     <= back     ", lstrlen(L"     <= back     "));
	}
}
void UIMenu(int SelectedButton, const TCHAR String0[30], const TCHAR String1[30], const TCHAR String2[30], const TCHAR String3[30], const TCHAR String4[30], int RGB[9]) {
	HFONT hFont, oldFont;
	hFont = CreateFont((int)(300 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);

	int rgb[4];
	for (int i = 1; i < 4; i++)
	{
		rgb[i] = RGBConverter(RGB[0], i);
	}
	SetTextColor(memdc, RGB(rgb[1], rgb[2], rgb[3]));
	TCHAR lpOut[100];
	swprintf_s(lpOut, 100, L"%s", String0);
	TextOut(memdc, int(Pibot_x - 800 * window_size), int(Pibot_y - 500 * window_size), lpOut, lstrlen(lpOut));

	hFont = CreateFont((int)(150 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);

	switch (SelectedButton)
	{
	case 1:
		UIButton(-650, -125, RGB[5], true, String1);
		UIButton(-650, 25, RGB[2], false, String2);
		UIButton(-650, 175, RGB[3], false, String3);
		UIButton(-650, 325, RGB[4], false, String4);
		break;
	case 2:
		UIButton(-650, -125, RGB[1], false, String1);
		UIButton(-650, 25, RGB[6], true, String2);
		UIButton(-650, 175, RGB[3], false, String3);
		UIButton(-650, 325, RGB[4], false, String4);
		break;
	case 3:
		UIButton(-650, -125, RGB[1], false, String1);
		UIButton(-650, 25, RGB[2], false, String2);
		UIButton(-650, 175, RGB[7], true, String3);
		UIButton(-650, 325, RGB[4], false, String4);
		break;
	case 4:
		UIButton(-650, -125, RGB[1], false, String1);
		UIButton(-650, 25, RGB[2], false, String2);
		UIButton(-650, 175, RGB[3], false, String3);
		UIButton(-650, 325, RGB[8], true, String4);
		break;
	default:
		UIButton(-650, -125, RGB[1], false, String1);
		UIButton(-650, 25, RGB[2], false, String2);
		UIButton(-650, 175, RGB[3], false, String3);
		UIButton(-650, 325, RGB[4], false, String4);
		break;
	}
	SelectObject(memdc, oldFont);
	DeleteObject(hFont);
}
void UIButton(int x, int y, int RGB, bool Seleted, const TCHAR String[30])
{
	int rgb[4];
	for (int i = 1; i < 4; i++)
	{
		rgb[i] = RGBConverter(RGB, i);
	}
	SetTextColor(memdc, RGB(rgb[1], rgb[2], rgb[3]));
	TCHAR lpOut[100];
	if (Seleted) swprintf_s(lpOut, 100, L"  \\%s", String);
	else swprintf_s(lpOut, 100, L"%s", String);
	TextOut(memdc, int(Pibot_x + x * window_size), int(Pibot_y + y * window_size), lpOut, lstrlen(lpOut));
}
bool UIButtonSelected(int x, int y, int sizex, int sizey, POINTS Mouse)
{
	int laterx = x + sizex, latery = y + sizey;

	return (Pibot_x + x * window_size < Mouse.x&&
		Mouse.x < Pibot_x + laterx * window_size &&
		Pibot_y + y * window_size < Mouse.y&&
		Mouse.y < Pibot_y + latery * window_size);
}
void UIEndMessage()
{
	TCHAR lpOut[100];
	HFONT hFont, oldFont;
	hFont = CreateFont((int)(200 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);

	if (TotalScore < 1000)
	{
		SetTextColor(memdc, RGB(0, 255, 0));
		swprintf_s(lpOut, 100, L"Score:\\%.0f ", TotalScore);
	}
	else if (TotalScore < 1000000)
	{
		if (TotalScore < 10000) SetTextColor(memdc, RGB(255, 255, 255));
		else if (TotalScore < 100000)  SetTextColor(memdc, RGB(255, 255, 0));
		else if (TotalScore < 500000)  SetTextColor(memdc, RGB(0, 255, 255));
		else  SetTextColor(memdc, RGB(255, 0, 255));
		swprintf_s(lpOut, 100, L"Score:\\%.3gK", TotalScore / 1000);
	}
	else if (TotalScore < 1000000000)
	{
		SetTextColor(memdc, RGB(255, 0, 0));
		swprintf_s(lpOut, 100, L"Score:\\%.3gM", TotalScore / 1000000);
	}
	else if (TotalScore < 1000000000000)
	{
		SetTextColor(memdc, RGB(0, 0, 255));
		swprintf_s(lpOut, 100, L"Score:\\%.3gB", TotalScore / 1000000000);
	}
	else
	{
		SetTextColor(memdc, RGB(255, 192, 0));
		swprintf_s(lpOut, 100, L"Score:\\MAX\\");
	}

	TextOut(memdc, int(Pibot_x - 400 * window_size), int(Pibot_y + 70 * window_size), lpOut, lstrlen(lpOut));
	TextOut(memdc, int(Pibot_x - 400 * window_size), int(Pibot_y - 210 * window_size), L"Game Over", lstrlen(L"Game Over"));

	SelectObject(memdc, oldFont);
	DeleteObject(hFont);
}
void UIScore()
{
	TCHAR lpOut[100];
	HFONT hFont, oldFont;
	hFont = CreateFont((int)(100 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);
	SetTextColor(memdc, RGB(0, 255, 0));
	if (Score < 1000) swprintf_s(lpOut, 100, L"\\%.0f ", Score);
	else if (Score < 1000000) swprintf_s(lpOut, 100, L"\\%.3gK", Score / 1000);
	else if (Score < 1000000000) swprintf_s(lpOut, 100, L"\\%.3gM", Score / 1000000);
	else if (Score < 1000000000000) swprintf_s(lpOut, 100, L"\\%.3gB", Score / 1000000000);
	else swprintf_s(lpOut, 100, L"\\MAX\\");

	TextOut(memdc, int(Pibot_x + 600 * window_size), int(Pibot_y), lpOut, lstrlen(lpOut));
	TextOut(memdc, int(Pibot_x + 600 * window_size), int(Pibot_y - 75 * window_size), L"Power", 5);
	if (Orbcount < 0) {
		SetTextColor(memdc, RGB(255, 0, 0));
		swprintf_s(lpOut, 100, L"X");
	}
	else swprintf_s(lpOut, 100, L"%d", Orbcount);
	TextOut(memdc, int(Pibot_x + 725 * window_size), int(Pibot_y - 367.5 * window_size), lpOut, lstrlen(lpOut));

	SelectObject(memdc, oldFont);
	DeleteObject(hFont);
}
void UIDebugInfo()
{
	TCHAR lpOut[100];
	SetTextColor(memdc, RGB(0, 0, 0));
	swprintf_s(lpOut, 100, L"Orb : %g ", OrbHead->next->angle);
	TextOut(memdc, 0, 0, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"x : %g ", OrbHead->next->x);
	TextOut(memdc, 0, 12, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"y : %g ", OrbHead->next->y);
	TextOut(memdc, 0, 25, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"relative : %g ", AnglePosition(OrbHead->next->x, OrbHead->next->y));
	TextOut(memdc, 0, 37, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"reflector : %g ", Player[0].Reflector.angle);
	TextOut(memdc, 0, 50, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"speed : %g ", Player[0].Reflector.speed);
	TextOut(memdc, 0, 62, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"age : %d ", Player[0].Reflector.age);
	TextOut(memdc, 0, 75, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"position : %g ", Player[0].Reflector.position);
	TextOut(memdc, 0, 87, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"Time : %d ", Time);
	TextOut(memdc, 0, 100, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"Cherenkov : %d ", Reactor.cherenkovmeter);
	TextOut(memdc, 0, 112, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"Cherenkov counter : %d ", Reactor.cherenkovcounter);
	TextOut(memdc, 0, 125, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"Temperture : %g ", Temperture);
	TextOut(memdc, 0, 137, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"Mole : %g ", Mole);
	TextOut(memdc, 0, 150, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"distance : %g ", sqrt(OrbHead->next->x * OrbHead->next->x + OrbHead->next->y * OrbHead->next->y));
	TextOut(memdc, 0, 162, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"effect : %g ", OrbHead->next->afterx[0]);
	TextOut(memdc, 0, 175, lpOut, lstrlen(lpOut));
	return;
}
void UIGameStatusDebugInfo(int GameStatus, int EscMode, bool DisplayGame)
{
	TCHAR lpOut[100];

	HFONT hFont, oldFont;
	hFont = CreateFont((int)(25 * window_size), 0, 0, 0, FW_ULTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"PowerIngElectric");
	oldFont = (HFONT)SelectObject(memdc, hFont);

	SetTextColor(memdc, RGB(0, 0, 0));
	swprintf_s(lpOut, 100, L"animation : %d ", AnimationTime_Door);
	TextOut(memdc, 0, 0, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"GameStatus : %d ", GameStatus);
	TextOut(memdc, 0, 12, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"reflector : %g ", Player[0].Reflector.angle);
	TextOut(memdc, 0, 25, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"meltdown : %d ", Reactor.meltdownlevel);
	TextOut(memdc, 0, 37, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"esc : %d ", EscMode);
	TextOut(memdc, 0, 50, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"orb : %d ", Orbcount);
	TextOut(memdc, 0, 62, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"display : %d ", DisplayGame);
	TextOut(memdc, 0, 75, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"cherenkov : %d ", Reactor.cherenkovlevel);
	TextOut(memdc, 0, 87, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L"effect : %d ", Player[0].Reflector.effect);
	TextOut(memdc, 0, 100, lpOut, lstrlen(lpOut));
	/*
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 112, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 125, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 137, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 150, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 162, lpOut, lstrlen(lpOut));
	swprintf_s(lpOut, 100, L" : % ");
	TextOut(memdc, 0, 175, lpOut, lstrlen(lpOut));
	*/

	SelectObject(memdc, oldFont);
	DeleteObject(hFont);

	return;
}
//--------------------------------------------------------------------------------------------------------------//
POINT RotatePaint1(double x, double y, double sizex, double sizey, double angle)
{
	return{ (long)(Pibot_x + x * window_size + PointRotationX(-sizex * window_half, -sizey * window_half, angle)), (long)(Pibot_y + y * window_size + PointRotationY(-sizex * window_half, -sizey * window_half, angle)) };
}
POINT RotatePaint2(double x, double y, double sizex, double sizey, double angle)
{
	return{ (long)(Pibot_x + x * window_size + PointRotationX(sizex * window_half, -sizey * window_half, angle)), (long)(Pibot_y + y * window_size + PointRotationY(sizex * window_half, -sizey * window_half, angle)) };
}
POINT RotatePaint3(double x, double y, double sizex, double sizey, double angle)
{
	return{ (long)(Pibot_x + x * window_size + PointRotationX(-sizex * window_half, sizey * window_half, angle)), (long)(Pibot_y + y * window_size + PointRotationY(-sizex * window_half, sizey * window_half, angle)) };
}
//--------------------------------------------------------------------------------------------------------------//
POINT ReflectorPaint1(struct Power_Reflector_Pointless Reflector, double Vertical)
{
	return{ (long)(Pibot_x + PointRotationX(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) - Reflector_half_y + (25 - Vertical) * window_size, Reflector_half_x, Reflector.angle)),(long)(Pibot_y + PointRotationY(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) - Reflector_half_y + (25 - Vertical) * window_size, Reflector_half_x, Reflector.angle)) };
}
POINT ReflectorPaint2(struct Power_Reflector_Pointless Reflector, double Vertical)
{
	return{ (long)(Pibot_x + PointRotationX(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) - Reflector_half_y + (25 - Vertical) * window_size, -Reflector_half_x, Reflector.angle)),(long)(Pibot_y + PointRotationY(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) - Reflector_half_y + (25 - Vertical) * window_size, -Reflector_half_x, Reflector.angle)) };
}
POINT ReflectorPaint3(struct Power_Reflector_Pointless Reflector, double Vertical)
{
	return{ (long)(Pibot_x + PointRotationX(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) + Reflector_half_y + (25 + Vertical) * window_size, Reflector_half_x, Reflector.angle)),(long)(Pibot_y + PointRotationY(window_size * sqrt((Reflector.position + Reflector.rebound) * (Reflector.position + Reflector.rebound) - Reflector.size * Reflector.size * 0.25) + Reflector_half_y + (25 + Vertical) * window_size, Reflector_half_x, Reflector.angle)) };
}