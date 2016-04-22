#pragma once

#define WINVER 0x0501
#include <windows.h>
#include <winuser.rh> // for OCR_*
#include <memory>
#include "resource.h"
#include "OptionsDlg.h"
#include "Switcher.h"
#include "AeroHelper.h"


#define TIMER_ID 5698
#define TRAY_ICON_ID 5812
#define WM_USER_TRAYICON (WM_USER + 1)

static UINT WM_TaskbarCreated;

class AppDlg
{
public:
	AppDlg(HINSTANCE hInst, Switcher& pSwitcher, OptionsHelper& oh) : _hInst(hInst), _pSwitcher(pSwitcher), _oh(oh)
	{
	}

	~AppDlg()
	{		
	}

	void ShowDlg()
	{
		if (_bShowingDlg) return;

		_bShowingDlg = true;
		DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_MAIN_DLG), NULL, 
			reinterpret_cast<DLGPROC>(AppDlgWndProc),
			reinterpret_cast<LPARAM>(this));
		_bShowingDlg = false;
	}

private:
	NOTIFYICONDATA _nidTray;
	HWND _hwnd = NULL;
	HINSTANCE _hInst = NULL;
	HICON _hMainIco = NULL;
	HICON _hRuIco = NULL;
	HICON _hEnIco = NULL;
	HCURSOR _hRuCur = NULL;
	HCURSOR _hEnCur = NULL;
	HCURSOR _hDefCur = NULL;
	HMENU _hTrayMenu = NULL;
	HKL dwLastLayout = NULL;
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

		return;
	}

	void RemoveTrayIcon()
	{
		Shell_NotifyIcon(NIM_DELETE, &_nidTray);

		return;
	}

	void inline SetTrayIcon(HICON hIcon)
	{
		_nidTray.hIcon = hIcon;
		Shell_NotifyIcon(NIM_MODIFY, &_nidTray);
	}

	void inline SetIBeamCursor(HCURSOR hCursor)
	{
		HCURSOR hCopy = CopyCursor(hCursor);
		SetSystemCursor(hCopy, OCR_IBEAM);
		DestroyCursor(hCopy);
	}

	static LRESULT CALLBACK AppDlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		AppDlg* pThis = reinterpret_cast<AppDlg*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (message == WM_TaskbarCreated)
		{
			pThis->dwLastLayout = 0;
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

			pThis->_optionsDlg = std::unique_ptr<OptionsDlg>(new OptionsDlg(pThis->_hInst, pThis->_oh, pThis->_pSwitcher));

			pThis->_hMainIco = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN_ICON));
			pThis->_hRuIco = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_RUS));
			pThis->_hEnIco = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ENG));
			pThis->_hRuCur = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_RUS1));
			pThis->_hEnCur = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_ENG1));
			pThis->_hDefCur = CopyCursor(LoadCursor(NULL, MAKEINTRESOURCE(OCR_IBEAM)));	
			pThis->_hTrayMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAY_MENU));

			pThis->_hwnd = hWnd;
			SetClassLongPtr(hWnd, GCLP_HICON, reinterpret_cast<LONG>(pThis->_hMainIco));
			
			pThis->AddTrayIcon();

			SetTimer(hWnd, TIMER_ID, 500, 0);

			WM_TaskbarCreated = RegisterWindowMessage(_T("TaskbarCreated"));

			return TRUE;
		}

		case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* wp = (WINDOWPOS*)lParam;
			wp->flags &= ~SWP_SHOWWINDOW;
			return TRUE;
		}

		case WM_DESTROY:
		{
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
				TrackPopupMenu(GetSubMenu(pThis->_hTrayMenu, 0), TPM_RIGHTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, pThis->_hwnd, NULL);
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

		case WM_TIMER:
		{
			if (wParam == TIMER_ID)
			{
				POINT p;
				HWND wnd;
				HKL lay = 0;
				
				if (pThis->_oh.IsTrackActiveWndByMouse())
				{
					GetCursorPos(&p);
					wnd = WindowFromPoint(p);
				}
				else wnd = GetForegroundWindow();

				if (!IsWindow(wnd)) break;

				lay = GetKeyboardLayout(GetWindowThreadProcessId(wnd, 0));
				if (pThis->dwLastLayout == lay) break;

				if ((DWORD)lay == 0x4190419/*RU*/) 
				{
					pThis->SetTrayIcon(pThis->_hRuIco);
					if (pThis->_oh.IsChangeMousePointer()) pThis->SetIBeamCursor(pThis->_hRuCur);
					if (pThis->_oh.IsChangeColor()) pThis->_ah.SetColor(0, 0, 200);
					if (pThis->_oh.IsFlashScrollLock()) pThis->_kh.FlashScrollLockLed(true);
				}
				else
					if ((DWORD)lay == 0x4090409/*EN*/)
					{
						pThis->SetTrayIcon(pThis->_hEnIco);
						if (pThis->_oh.IsChangeMousePointer()) pThis->SetIBeamCursor(pThis->_hEnCur);
						if (pThis->_oh.IsChangeColor()) pThis->_ah.SetColor(200, 0, 0);
						if (pThis->_oh.IsFlashScrollLock()) pThis->_kh.FlashScrollLockLed(false);
					}
					else
					{
						// раскладка не та
						LOG("unsupported keyboard layout 0x" << std::hex << lay);
						pThis->SetTrayIcon(pThis->_hMainIco);
						if (pThis->_oh.IsChangeMousePointer()) pThis->SetIBeamCursor(pThis->_hDefCur);
						if (pThis->_oh.IsChangeColor()) pThis->_ah.RestoreColor();
					}

				pThis->dwLastLayout = lay;

				return TRUE;
			}

			break;
		}

		default:
			return FALSE;
		}


		return FALSE;
	}

};






