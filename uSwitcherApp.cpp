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
bool globalDebugFlag = true;

LRESULT CALLBACK SwitcherKeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_bHookActive && nCode == HC_ACTION)
	{
		auto *ks = (KBDLLHOOKSTRUCT*)lParam;
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
	if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN ||
		wParam == WM_LBUTTONDBLCLK || wParam == WM_RBUTTONDBLCLK || wParam == WM_MBUTTONDBLCLK)
	{
		g_switcher->BeginNewWord();
		g_switcher->ProcessKeyPress(0xDA, WM_KEYDOWN, lParam);
		g_switcher->ProcessKeyPress(0xDA, WM_KEYUP, lParam);
		// эмулируем нажатие кнопки для switcher. если хоткей установлен как rctrl, например, а мы нажимаем ctrl-лкм, то срабатывает событие хоткея, хотя и не должно.
	}

	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	wchar_t wsIni[512] = { 0 };
	GetModuleFileName(hInstance, wsIni, 511);
	PathRemoveExtension(wsIni);
	wcscat(wsIni, L".ini");

	wchar_t wcRunPath[512] = { 0 };
	GetModuleFileName(hInstance, wcRunPath, 511);
	PathRemoveFileSpecW(wcRunPath);

	wchar_t wcExe[512] = { 0 };
	GetModuleFileName(hInstance, &wcExe[0], 511);

	OptionsHelper oh(wsIni, wcExe);
	SoundHelper sh(std::wstring(wcRunPath) + std::wstring(L"\\resources\\error.wav"), std::wstring(wcRunPath) + std::wstring(L"\\resources\\switch.wav"));

	g_switcher = std::make_unique<Switcher>(oh, sh);

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