#pragma once

#include <windows.h>
#include <deque>
#include <string>
#include <functional>
#include "KeyboardHelper.h"
#include "OptionsHelper.h"
#include "log.h"
#include "SoundHelper.h"
#include "SelectionConverter.h"
#include "LastWordConverter.h"
#include "SwitchLang.h"

#define BUFFER_LENGTH 1024


class Switcher 
{
public:
	Switcher(OptionsHelper& oh, SoundHelper& sh) : _oh(oh), _sh(sh)
	{
	}

	Switcher::~Switcher()
	{
	}

	void BeginNewWord() { _lwc.BeginNewWord(); }
	void Enable() { _bEnabled = true; }
	void Disable() { _bEnabled = false; }
	void RecordModeOn() { _bRecordMode = true; _keRecord = KeyboardEvent(); }
	void RecordModeOff() { _bRecordMode = false; }
	KeyboardEvent GetRecordedEvent() { return _keRecord; }
	void HotkeyComboBreaker() { _bHotkeyFlag = false; }

	// returns true if event processed and no need to call next hook
	bool ProcessKeyPress(DWORD vkCode, WPARAM wParam, LPARAM lParam)
	{
		if (_bRecordMode)
		{
			if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) _keRecord = _kh.FillKeybardEvent(vkCode);
			LOG("rec 0x" << std::hex << _keRecord._vkCode);
			return false;
		}

		if (_bEnabled == false) return false;

		KeyboardEvent ke = _kh.FillKeybardEvent(vkCode);

		LOG(L"key pressed: " << ke.ToString() << L", wParam 0x" << std::hex << wParam);

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			_bHotkeyFlag = true;
		}

		if (ke == _oh.KeyConvertLast())
		{
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::ConvertLastWord, this, std::placeholders::_1));
		}

		if (ke == _oh.KeyConvertSelection())
		{
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::ConvertSelection, this, std::placeholders::_1));
		}

		if (ke == _oh.KeySwitchToEn() && _oh.KeySwitchToEn() == _oh.KeySwitchToRu())
		{
			return ProcessHotkey(ke, wParam, std::bind(&SwitchLangNext, GetFocusedHandle(GetForegroundWindow())));
		}

		if (ke == _oh.KeySwitchToEn())
		{
			return ProcessHotkey(ke, wParam, std::bind(&SwitchLangEng, GetFocusedHandle(GetForegroundWindow())));
		}

		if (ke == _oh.KeySwitchToRu())
		{
			return ProcessHotkey(ke, wParam, std::bind(&SwitchLangRus, GetFocusedHandle(GetForegroundWindow())));
		}

		if (ke == _oh.KeySearchInet())
		{
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::SearchInet, this, std::placeholders::_1));
		}


		//if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) return false;
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) return false;

		// scroll lock
		if (ke == KeyboardEvent(VK_SCROLL))
		{
			KBDLLHOOKSTRUCT* pKhs = (KBDLLHOOKSTRUCT*)lParam;

			// if we process this
			if (_oh.IsFlashScrollLock() == false) return false;

			if ((pKhs->flags & LLKHF_INJECTED) != LLKHF_INJECTED)
			{
				// scroll lock pressed from keyboard, not injected, so we return "ok, we processed it, no need to do anything"
				return true;
			}

			// scroll lock injected (hope from uswitcher), pass this action to next hook in chain to switch on key led
			return false;
		}

		// caps lock
		if (ke == KeyboardEvent(VK_CAPITAL))
		{
			KBDLLHOOKSTRUCT* pKhs = (KBDLLHOOKSTRUCT*)lParam;

			if (ke != _oh.KeySwitchToEn() || ke != _oh.KeySwitchToRu()) return false;

			if ((pKhs->flags & LLKHF_INJECTED) != LLKHF_INJECTED)
			{
				if (ke != _oh.KeySwitchToEn()) return ProcessHotkey(ke, wParam, std::bind(&SwitchLangEng, std::placeholders::_1));
				if (ke != _oh.KeySwitchToRu()) return ProcessHotkey(ke, wParam, std::bind(&SwitchLangRus, std::placeholders::_1));
			}
		}

		_lwc.AppendBuffer(ke);

		return false;
	}
	

private:
	bool _bEnabled = false;
	KeyboardHelper _kh;
	OptionsHelper& _oh;
	SoundHelper& _sh;
	bool _bRecordMode = false;
	KeyboardEvent _keRecord;
	SelectionConverter _sc;
	bool _bHotkeyFlag = false;
	LastWordConverter _lwc;

	bool ProcessHotkey(KeyboardEvent& ke, WPARAM wParam, std::function<void(HWND)> fWorker)
	{
		// так не работает, потому что для хоткеев с модификатором и кодом приходят сообщения раздельно и этот код работает только если отпускать кнопки в нужном порядке
		/*
		if (_keHotkey == ke)
		{
			LOG("hotkey " << ke.ToString() << " detected");
			fWorker(GetForegroundWindow());
		}

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) _keHotkey = ke;
		*/

		if (ke.Modified() && ke._vkCode == 0)
		{
			// это хоткей из одних модификаторов

			if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
			{
				_bHotkeyFlag = false;
				return false;
			}

			if (_bHotkeyFlag == true)
			{
				// не наш хоткей
				LOG("not hotkey");
				_bHotkeyFlag = false;
				return false;
			}
		}
		else
		{
			if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
			{
				return false;
			}
		}

		fWorker(GetForegroundWindow());

		_bHotkeyFlag = false;

		if (ke.Modified() && ke._vkCode == 0) return false;
		return true;

		return false;
	}

	void ConvertLastWord(HWND hWnd)
	{
		LOG("converting last word");

		HWND wndFocused = GetFocusedHandle(hWnd);

		_kh.UnpressKey(_oh.KeyConvertLast());

		if (_lwc.ConvertLastWord(wndFocused) == false)
		{
			if (_oh.IsPlaySound()) _sh.PlayError();
		}
		else
		{
			if (_oh.IsPlaySound()) _sh.PlaySwitch();
		}
	}

	void ConvertSelection(HWND hWnd)
	{
		LOG("converting selection");

		HWND wndFocused = GetFocusedHandle(hWnd);

		_kh.UnpressKey(_oh.KeyConvertSelection());

		if (_sc.ConvertSelection(wndFocused) == false)
		{
			if (_oh.IsPlaySound()) _sh.PlayError();
		}
		else
		{
			if (_oh.IsPlaySound()) _sh.PlaySwitch();
		}

		SwitchLangNext(wndFocused);

		_lwc.BeginNewWord();

		return;
	}

	HWND GetFocusedHandle(HWND wndForeground)
	{
		int  idActive = GetWindowThreadProcessId(wndForeground, NULL);
		if (AttachThreadInput(GetCurrentThreadId(), idActive, TRUE) == NULL) return wndForeground;

		HWND wndFocused = GetFocus();

		AttachThreadInput(GetCurrentThreadId(), idActive, FALSE);

		if (IsWindow(wndFocused) == FALSE)
		{
			return wndForeground;
		}

		return wndFocused;
	}

	void SearchInet(HWND hwnd)
	{
		auto Query = _oh.SearchQuery();
		if (Query.find(L"http://") == std::wstring::npos)
			Query = L"http://" + Query;

		auto SelText = _sc.GetSelectionText(GetFocusedHandle(hwnd));
		if (SelText.empty())
		{
			if (_oh.IsPlaySound()) _sh.PlayError();
			return;
		}

		std::wstring Mask = L"%s";

		size_t StartPos = Query.find(Mask);
		if (StartPos == std::string::npos)
			Query += SelText;
		else Query.replace(StartPos, Mask.length(), SelText);

		LOG("starting inet search: " << Query);

		ShellExecute(GetDesktopWindow(), L"open", Query.c_str(), NULL, NULL, SW_SHOW);

		_lwc.BeginNewWord();
	}
};
