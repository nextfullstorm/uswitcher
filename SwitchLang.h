#pragma once
#include <windows.h>

static inline void SwitchLangNext(HWND hWndFocused)
{
	LOG("switching to NEXT");
	PostMessage(hWndFocused, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, HKL_NEXT);
	Sleep(100);
}

static inline void SwitchLangRus(HWND hWndFocused)
{
	LOG("switching to RU");
	PostMessage(hWndFocused, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)0x04190419);
}

static inline void SwitchLangEng(HWND hWndFocused)
{
	LOG("switching to EN");
	PostMessage(hWndFocused, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)0x04090409);
}
