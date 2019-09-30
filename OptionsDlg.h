#pragma once
#include <windows.h>
#include "resource.h"
#include "OptionsHelper.h"
#include "log.h"
#include <windowsx.h>
#include "Switcher.h"
#include <map>

#define TRAY_ICON_ID 5812
#define WM_USER_TRAYICON (WM_USER + 1)

static const wchar_t wcRec[2] = L"\x25CF";
static const wchar_t wcStop[2] = L"\x25A0";

static const int nTimerId = 5663;

class OptionsDlg
{
public:
	OptionsDlg(const HINSTANCE hInst, OptionsHelper& oh, Switcher& sw) : _oh(oh), _sw(sw)
	{
		_hInst = hInst;
	}

	~OptionsDlg()
	{
		
	}

	void ShowDlg()
	{
		if (_bShowingDlg)
		{
			return;
		}

		_bShowingDlg = true;
		DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_OPTIONS_DLG), nullptr,
			reinterpret_cast<DLGPROC>(OptionsDlgWndProc),
			reinterpret_cast<LPARAM>(this));
		_bShowingDlg = false;
	}

private:
	HWND _hwnd = nullptr;
	HINSTANCE _hInst = nullptr;
	bool _bShowingDlg = false;
	OptionsHelper& _oh;
	Switcher& _sw;
	bool _bRec = false;
	std::map<int, KeyboardEvent> _hks;
	int _nEditId = 0;
	int _nButtonId = 0;

private:
	static LRESULT CALLBACK OptionsDlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* pThis = reinterpret_cast<OptionsDlg*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
		case WM_INITDIALOG:
		{
			SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			pThis = reinterpret_cast<OptionsDlg*>(lParam);

			pThis->_hwnd = hWnd;
			SetClassLongPtr(hWnd, GCLP_HICON, reinterpret_cast<LONG>(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MAIN_ICON))));

			LOGFONT lf = { 0 };
			lf.lfHeight = 40;
			lf.lfWeight = 800;
			lf.lfQuality = CLEARTYPE_QUALITY;
			wcscpy(lf.lfFaceName, L"Arial");
			HFONT hFnt = CreateFontIndirect(&lf);

			SendMessage(GetDlgItem(hWnd, IDC_LOGO_TXT), WM_SETFONT, (WPARAM)hFnt, TRUE);

			ShowWindow(hWnd, SW_RESTORE);
			SetActiveWindow(hWnd);

			SetWindowText(GetDlgItem(pThis->_hwnd, IDC_REC1), wcRec);

			pThis->LoadSettings();

			return TRUE;
		}

		case WM_DESTROY:
		{
			return TRUE;
		}

		case WM_CLOSE:
		{
			EndDialog(hWnd, 0);
			return TRUE;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			LOG("key code 0x" << std::hex << wParam);
			break;
		}

		case WM_COMMAND:
		{
			switch (wParam)
			{
			case IDC_SAVE:
				{
					pThis->SaveSettings();

					EndDialog(hWnd, 0);

					break;
				}

			case IDC_CLOSE:
				{
					EndDialog(hWnd, 0);
					break;
				}

			case IDC_REC1:
				{
					pThis->OnPressRec(IDC_REC1, IDC_HK_LAST);
					break;
				}

			case IDC_REC2:
				{
					pThis->OnPressRec(IDC_REC2, IDC_HK_SELECT);
					break;
				}

			case IDC_REC3:
				{
					pThis->OnPressRec(IDC_REC3, IDC_HK_EN);
					break;
				}

			case IDC_REC4:
				{
					pThis->OnPressRec(IDC_REC4, IDC_HK_RU);
					break;
				}

			case IDC_REC5:
				{
					pThis->OnPressRec(IDC_REC5, IDC_HK_SEARCH);
					break;
				}

			}			

			return TRUE;
		}

		case WM_TIMER:
		{
			if (wParam != nTimerId) break;

			if (pThis->_nEditId)
			{
				if (pThis->_sw.GetRecordedEvent().Valid())
				{
					pThis->_hks[pThis->_nEditId] = pThis->_sw.GetRecordedEvent();
					pThis->OnPressRec(pThis->_nButtonId, pThis->_nEditId);
					KillTimer(pThis->_hwnd, nTimerId);
				}
			}
		}

		default:
			break;
		}

		return FALSE;
	}

	void OnPressRec(const int nCtrlIdButton, const int nCtrlIdEdit)
	{
		if (_bRec == false)
		{
			_sw.RecordModeOn();
			_bRec = true;
			SetDlgItemText(_hwnd, nCtrlIdEdit, L"жду нажатие");
			SetWindowText(GetDlgItem(_hwnd, nCtrlIdButton), wcStop);

			EnableWindow(GetDlgItem(_hwnd, IDC_REC1), FALSE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC2), FALSE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC3), FALSE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC4), FALSE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC5), FALSE);

			EnableWindow(GetDlgItem(_hwnd, nCtrlIdButton), TRUE);

			_nButtonId = nCtrlIdButton;
			_nEditId = nCtrlIdEdit;

			SetTimer(_hwnd, nTimerId, 1000, nullptr);
		}
		else
		{
			_sw.RecordModeOff();

			EnableWindow(GetDlgItem(_hwnd, IDC_REC1), TRUE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC2), TRUE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC3), TRUE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC4), TRUE);
			EnableWindow(GetDlgItem(_hwnd, IDC_REC5), TRUE);

			SetWindowText(GetDlgItem(_hwnd, nCtrlIdButton), wcRec);

			SetDlgItemText(_hwnd, IDC_HK_LAST, _hks[IDC_HK_LAST].ToString().c_str());
			SetDlgItemText(_hwnd, IDC_HK_SELECT, _hks[IDC_HK_SELECT].ToString().c_str());
			SetDlgItemText(_hwnd, IDC_HK_EN, _hks[IDC_HK_EN].ToString().c_str());
			SetDlgItemText(_hwnd, IDC_HK_RU, _hks[IDC_HK_RU].ToString().c_str());
			SetDlgItemText(_hwnd, IDC_HK_SEARCH, _hks[IDC_HK_SEARCH].ToString().c_str());

			_bRec = false;

			KillTimer(_hwnd, nTimerId);
		}
	}

	void LoadSettings()
	{
		_oh.LoadOptions();

		if (_oh.IsAutostart()) CheckDlgButton(_hwnd, IDC_ISAUTOSTART, TRUE);
		if (_oh.IsChangeColor()) CheckDlgButton(_hwnd, IDC_ISCHANGECOLOR, TRUE);
		if (_oh.IsChangeMousePointer()) CheckDlgButton(_hwnd, IDC_ISMOUSECURSOR, TRUE);
		if (_oh.IsFlashScrollLock()) CheckDlgButton(_hwnd, IDC_ISSCROLLLOCK, TRUE);
		if (_oh.IsPlaySound()) CheckDlgButton(_hwnd, IDC_ISSOUND, TRUE);
		SetDlgItemText(_hwnd, IDC_SEARCHSTRING, _oh.SearchQuery().c_str());

		SetDlgItemText(_hwnd, IDC_HK_LAST, _oh.KeyConvertLast().ToString().c_str());
		SetDlgItemText(_hwnd, IDC_HK_SELECT, _oh.KeyConvertSelection().ToString().c_str());
		SetDlgItemText(_hwnd, IDC_HK_EN, _oh.KeySwitchToEn().ToString().c_str());
		SetDlgItemText(_hwnd, IDC_HK_RU, _oh.KeySwitchToRu().ToString().c_str());
		SetDlgItemText(_hwnd, IDC_HK_SEARCH, _oh.KeySearchInet().ToString().c_str());

		_hks[IDC_HK_LAST] = _oh.KeyConvertLast();
		_hks[IDC_HK_SELECT] = _oh.KeyConvertSelection();
		_hks[IDC_HK_EN] = _oh.KeySwitchToEn();
		_hks[IDC_HK_RU] = _oh.KeySwitchToRu();
		_hks[IDC_HK_SEARCH] = _oh.KeySearchInet();

		CheckDlgButton(_hwnd, IDC_CURSORACTIVEWIN, !(_oh.IsTrackActiveWndByMouse()));
		CheckDlgButton(_hwnd, IDC_CURSORANYWIN, _oh.IsTrackActiveWndByMouse());
	}

	void SaveSettings()
	{
		_oh.SetAutostart(IsDlgButtonChecked(_hwnd, IDC_ISAUTOSTART) == BST_CHECKED);
		_oh.SetChangeColor(IsDlgButtonChecked(_hwnd, IDC_ISCHANGECOLOR) == BST_CHECKED);
		_oh.SetChangeMousePointer(IsDlgButtonChecked(_hwnd, IDC_ISMOUSECURSOR) == BST_CHECKED);
		_oh.SetFlashScrollLock(IsDlgButtonChecked(_hwnd, IDC_ISSCROLLLOCK) == BST_CHECKED);
		_oh.SetPlaySound(IsDlgButtonChecked(_hwnd, IDC_ISSOUND) == BST_CHECKED);

		std::wstring wsTxt;
		wsTxt.resize(200);
		GetDlgItemText(_hwnd, IDC_SEARCHSTRING, &wsTxt[0], 199);
		_oh.SetSearchQuery(wsTxt);

		_oh.SetKeyConvertLast(_hks[IDC_HK_LAST]);
		_oh.SetKeyConvertSelection(_hks[IDC_HK_SELECT]);
		_oh.SetKeySwitchToEn(_hks[IDC_HK_EN]);
		_oh.SetKeySwitchToRu(_hks[IDC_HK_RU]);
		_oh.SetKeySearchInet(_hks[IDC_HK_SEARCH]);

		_oh.SetTrackActiveWndByMouse(IsDlgButtonChecked(_hwnd, IDC_CURSORANYWIN) == BST_CHECKED);

		_oh.SaveOptions();
	}
};
