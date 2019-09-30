#pragma once

//#define WINVER 0x0501
#include <windows.h>
#include <winuser.h> // for OCR_*
#include <memory>
#include "resource.h"
#include "OptionsDlg.h"
#include "Switcher.h"
#include "AeroHelper.h"
#include "Layouts.h"


#define TIMER_ID 5698
#define TIMER_TO 500
#define TRAY_ICON_ID 5812
#define WM_USER_TRAYICON (WM_USER + 1)

static UINT WM_TaskbarCreated;

class AppDlg
{
public:
	AppDlg(const HINSTANCE hInst, Switcher& pSwitcher, OptionsHelper& oh) : _hInst(hInst), _pSwitcher(pSwitcher), _oh(oh)
	{
	}

	~AppDlg()
	{		
	}

	void ShowDlg()
	{
		if (_bShowingDlg) return;

		_bShowingDlg = true;
		DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_MAIN_DLG), nullptr, 
			reinterpret_cast<DLGPROC>(AppDlgWndProc),
			reinterpret_cast<LPARAM>(this));
		_bShowingDlg = false;
	}

private:
	NOTIFYICONDATA _nidTray = {0};
	HWND _hwnd = nullptr;
	HINSTANCE _hInst = nullptr;
	HICON _hMainIco = nullptr;
	HICON _hRuIco = nullptr;
	HICON _hEnIco = nullptr;
	HCURSOR _hRuCur = nullptr;
	HCURSOR _hEnCur = nullptr;
	HCURSOR _hDefCur = nullptr;
	HMENU _hTrayMenu = nullptr;
	HKL dwLastLayout = nullptr;
	Switcher& _pSwitcher;
	bool _bShowingDlg = false;
	std::unique_ptr<OptionsDlg> _optionsDlg;
	AeroHelper _ah;
	OptionsHelper& _oh;
	KeyboardHelper _kh;

private:
	void AddTrayIcon()
	{		
		_nidTray.cbSize = sizeof(NOTIFYICONDATA);
		_nidTray.hWnd = (HWND)_hwnd;
		_nidTray.uID = TRAY_ICON_ID;
		_nidTray.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		_nidTray.hIcon = _hMainIco;
		_nidTray.uCallbackMessage = WM_USER_TRAYICON;
		wcscpy(_nidTray.szTip, L"uSwitcher");
		Shell_NotifyIcon(NIM_ADD, &_nidTray);
	}

	void RemoveTrayIcon()
	{
		Shell_NotifyIcon(NIM_DELETE, &_nidTray);
	}

	void inline SetTrayIcon(const HICON hIcon)
	{
		_nidTray.hIcon = hIcon;
		Shell_NotifyIcon(NIM_MODIFY, &_nidTray);
	}

	void inline SetIBeamCursor(const HCURSOR hCursor)
	{
		HCURSOR hCopy = CopyCursor(hCursor);
		SetSystemCursor(hCopy, OCR_IBEAM);
		DestroyCursor(hCopy);
	}

	void OnLayoutChanged(const HKL& lay)
	{
		if (dwLastLayout == lay) return;

		if (lay == LAYOUT_RU)
		{
			SetTrayIcon(_hRuIco);
			if (_oh.IsChangeMousePointer()) SetIBeamCursor(_hRuCur);
			if (_oh.IsChangeColor()) _ah.SetColor(0, 0, 200);
			if (_oh.IsFlashScrollLock()) _kh.FlashScrollLockLed(true);
		}
		else
			if (lay == LAYOUT_EN)
			{
				SetTrayIcon(_hEnIco);
				if (_oh.IsChangeMousePointer()) SetIBeamCursor(_hEnCur);
				if (_oh.IsChangeColor()) _ah.SetColor(200, 0, 0);
				if (_oh.IsFlashScrollLock()) _kh.FlashScrollLockLed(false);
			}
			else
			{
				// раскладка не та
				LOG("unsupported keyboard layout 0x" << std::hex << lay);
				SetTrayIcon(_hMainIco);
				if (_oh.IsChangeMousePointer()) SetIBeamCursor(_hDefCur);
				if (_oh.IsChangeColor()) _ah.RestoreColor();
			}

		dwLastLayout = lay;
	}

	static LRESULT CALLBACK AppDlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* pThis = reinterpret_cast<AppDlg*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (message == WM_TaskbarCreated)
		{
			pThis->dwLastLayout = nullptr;
			pThis->RemoveTrayIcon();
			pThis->AddTrayIcon();
			return TRUE;
		}

		switch (message)
		{
		case WM_INITDIALOG:
		{
			SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			pThis = reinterpret_cast<AppDlg*>(lParam);

			pThis->_optionsDlg = std::make_unique<OptionsDlg>(pThis->_hInst, pThis->_oh, pThis->_pSwitcher);

			pThis->_hMainIco = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MAIN_ICON));
			pThis->_hRuIco = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_RUS));
			pThis->_hEnIco = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ENG));
			pThis->_hRuCur = LoadCursor(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDC_RUS1));
			pThis->_hEnCur = LoadCursor(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDC_ENG1));
			pThis->_hDefCur = CopyCursor(LoadCursor(nullptr, MAKEINTRESOURCE(OCR_IBEAM)));
			pThis->_hTrayMenu = LoadMenu(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_TRAY_MENU));

			pThis->_hwnd = hWnd;
			SetClassLongPtr(hWnd, GCLP_HICON, reinterpret_cast<LONG>(pThis->_hMainIco));
			
			pThis->AddTrayIcon();

			WM_TaskbarCreated = RegisterWindowMessage(_T("TaskbarCreated"));

			pThis->_pSwitcher.SetLayoutChangedCallback(std::bind(&AppDlg::OnLayoutChanged, pThis, std::placeholders::_1));

			return TRUE;
		}

		case WM_WINDOWPOSCHANGING:
		{
			auto* wp = (WINDOWPOS*)lParam;
			wp->flags &= ~SWP_SHOWWINDOW;
			return TRUE;
		}

		case WM_DESTROY:
		{
			pThis->_pSwitcher.SetLayoutChangedCallback(nullptr);

			KillTimer(hWnd, TIMER_ID);
			pThis->RemoveTrayIcon();
			pThis->SetIBeamCursor(pThis->_hDefCur);
			DestroyCursor(pThis->_hDefCur);
			DestroyMenu(pThis->_hTrayMenu);
			PostQuitMessage(0);

			return TRUE;
		}

		/*case WM_CLOSE:
		{
			ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		}*/

		case WM_USER_TRAYICON:
		{
			if (wParam != TRAY_ICON_ID) return FALSE;

			if (LOWORD(lParam) == WM_RBUTTONUP)
			{
				POINT lpClickPoint;

				GetCursorPos(&lpClickPoint);
				SetForegroundWindow(pThis->_hwnd);
				TrackPopupMenu(GetSubMenu(pThis->_hTrayMenu, 0), TPM_RIGHTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, pThis->_hwnd, nullptr);
				PostMessage(pThis->_hwnd, WM_NULL, 0, 0);
				return TRUE;
			}

			if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
			{
				PostMessage(pThis->_hwnd, WM_COMMAND, MAKEWPARAM(ID_ROOT_OPTIONS, 0), 0);
			}

			return FALSE;
		}

		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);

			if (wmEvent == 0)
			{
				switch (wmId)
				{
				case ID_ROOT_OPTIONS:
				{
					pThis->_pSwitcher.Disable();
					pThis->_optionsDlg->ShowDlg();
					pThis->_pSwitcher.Enable();
					return TRUE;
				}

				case ID_ROOT_EXIT:
					PostQuitMessage(0);
					return TRUE;

				default:
					break;
				}
			}

			return FALSE;
		}

		default:
			return FALSE;
		}


		return FALSE;
	}

};






