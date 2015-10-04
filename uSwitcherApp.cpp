#include <windows.h>
#include "resource.h"
#include "AppDlg.h"
#include "Switcher.h"
#include <shlwapi.h>
#include "log.h"
#include "SoundHelper.h"
#pragma comment(lib, "shlwapi.lib")


HHOOK g_hKbdHook;
HHOOK g_hMouseHook;
std::unique_ptr<Switcher> g_switcher;
bool g_bHookActive;

LRESULT CALLBACK SwitcherKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_bHookActive && nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*)lParam;
		bool bProcessed = false;

		g_bHookActive = false;
		bProcessed = g_switcher->ProcessKeyPress(ks->vkCode, wParam, lParam);
		if (bProcessed) LOG("processed");
		g_bHookActive = true;

		if (bProcessed) return 1;
	}

	return CallNextHookEx(g_hKbdHook, nCode, wParam, lParam);
}

LRESULT CALLBACK SwitcherMouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN)
		g_switcher->BeginNewWord();

	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	std::wstring wsIni(512, 0);
	GetModuleFileName(hInstance, &wsIni[0], 511);
	PathRemoveExtension(&wsIni[0]);
	wcscat(&wsIni[0], L".ini");

	std::wstring wcRunPath(512, 0);
	GetModuleFileName(hInstance, &wcRunPath[0], 511);
	PathRemoveFileSpecW(&wcRunPath[0]);

	std::wstring wcExe(512, 0);
	GetModuleFileName(hInstance, &wcExe[0], 511);

	OptionsHelper oh(wsIni, wcExe);
	SoundHelper sh(wcRunPath + L"\\resources\\error.wav", wcRunPath + L"\\resources\\switch.wav");

	g_switcher = std::unique_ptr<Switcher>(new Switcher(oh, sh));

	g_bHookActive = true;

	g_hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, SwitcherKeyboardHook, hInstance, NULL);
	g_switcher->Enable();
	// assert

	if (!IsDebuggerPresent()) g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, SwitcherMouseHook, hInstance, NULL);
	// assert

	AppDlg dlg(hInstance, *g_switcher, oh);
	dlg.ShowDlg();

	g_bHookActive = false;
	UnhookWindowsHookEx(g_hKbdHook);
	if (!IsDebuggerPresent()) UnhookWindowsHookEx(g_hMouseHook);

	oh.SaveOptions();

	return 0;
}