#pragma once
#include <windows.h>

typedef struct COLORIZATIONPARAMS
{
	COLORREF         clrColor;          //ColorizationColor
	COLORREF         clrAftGlow;	   //ColorizationAfterglow
	UINT             nIntensity;	   //ColorizationColorBalance -> 0-100
	UINT             clrAftGlowBal;    //ColorizationAfterglowBalance
	UINT		 clrBlurBal;       //ColorizationBlurBalance
	UINT		 clrGlassReflInt;  //ColorizationGlassReflectionIntensity
	BOOL             fOpaque;
};

HRESULT(WINAPI *DwmIsCompositionEnabled)(BOOL *pfEnabled);
HRESULT(WINAPI *DwmSetColorizationParameters) (COLORIZATIONPARAMS *colorparam, UINT unknown);
HRESULT(WINAPI *DwmGetColorizationParameters) (COLORIZATIONPARAMS *colorparam);

class AeroHelper
{
public:
	AeroHelper()
	{
		_hDwm = LoadLibrary(L"dwmapi.dll");
		if (_hDwm == NULL) return;

		*(FARPROC *)&DwmIsCompositionEnabled = GetProcAddress(_hDwm, "DwmIsCompositionEnabled");
		*(FARPROC *)&DwmGetColorizationParameters = GetProcAddress(_hDwm, (LPCSTR)127);
		*(FARPROC *)&DwmSetColorizationParameters = GetProcAddress(_hDwm, (LPCSTR)131);

		if (DwmIsCompositionEnabled == NULL || DwmGetColorizationParameters == NULL || DwmSetColorizationParameters == NULL)
			return;

		if (IsDwmEnabled())
			DwmGetColorizationParameters(&_cpBackup);

		_bOk = true;
	}
	~AeroHelper()
	{
		RestoreColor();

		if (_hDwm) FreeLibrary(_hDwm);
	}

	void SetColor(unsigned char r, unsigned char g, unsigned char b)
	{
		if (IsDwmEnabled() == FALSE) return;

		COLORIZATIONPARAMS cpNewColor = { 0 };

		DwmGetColorizationParameters(&cpNewColor);
		cpNewColor.clrColor = RGB(b, g, r);
		cpNewColor.nIntensity = 50;

		DwmSetColorizationParameters(&cpNewColor, 0);
	}

	void RestoreColor()
	{
		if (IsDwmEnabled())
			DwmSetColorizationParameters(&_cpBackup, 0);
	}

private:
	HMODULE _hDwm = NULL;
	bool _bOk = false;
	COLORIZATIONPARAMS _cpBackup;

	BOOL IsDwmEnabled()
	{
		BOOL bEnabled = FALSE;
		DwmIsCompositionEnabled(&bEnabled);
		return bEnabled;
	}
};