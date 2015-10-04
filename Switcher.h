#pragma once

#include <windows.h>
#include <deque>
#include "ClipboardHelper.h"
#include "KeyboardHelper.h"
#include "OptionsHelper.h"
#include <string>
#include "AeroHelper.h"
#include <functional>
#include "log.h"
#include "SoundHelper.h"

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


	bool ProcessKeyPress(DWORD vkCode, WPARAM wParam, LPARAM lParam);
	void BeginNewWord();
	void Enable() { _bEnabled = true; }
	void Disable() { _bEnabled = false; }
	void RecordModeOn() { _bRecordMode = true; _keRecord = KeyboardEvent(); }
	void RecordModeOff() { _bRecordMode = false; }
	KeyboardEvent GetRecordedEvent() { return _keRecord; }

private:
	bool _bEnabled = false;
	int _cursorPos = 0;
	std::deque<KeyboardEvent> _buffer;
	KeyboardHelper _kh;
	OptionsHelper& _oh;
	ClipboardHelper _ch;
	SoundHelper& _sh;
	bool _bHotkeyFlag = false;
	bool _bRecordMode = false;
	KeyboardEvent _keRecord;

	const wchar_t* _wcRusKeys = L"ЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮёйцукенгшщзхъфывапролджэячсмитьбю\0";
	const wchar_t* _wcEngKeys = L"~QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>`qwertyuiop[]asdfghjkl;'zxcvbnm,.\0";

	bool IsPrintable(DWORD vkCode);
	KeyboardEvent& GetLastKeyPressed();
	void ConvertLastWord(HWND hWnd);
	void ConvertSelection(HWND hWnd);
	void ConvertSelectionEdit(HWND hWnd);
	void ConvertSelectionOther(HWND hWnd);

	void ConvertString(std::wstring& str);
	void DumpBuffer();

	void SwitchLangNext(HWND hWnd);
	void SwitchLangRus(HWND hwnd);
	void SwitchLangEng(HWND hwnd);
	void SearchInet(HWND hwnd);

	HWND GetFocusedHandle(HWND wndForeground);

	bool Process(KeyboardEvent& ke, WPARAM wParam, std::function<void(HWND)> foo);

	void AppendBuffer(KeyboardEvent& ke);
};


bool Switcher::Process(KeyboardEvent& ke, WPARAM wParam, std::function<void(HWND)> fWorker)
{
	
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
}


// returns true if event processed and no need to call next hook
bool Switcher::ProcessKeyPress(DWORD vkCode, WPARAM wParam, LPARAM lParam) 
{
	if (_bRecordMode)
	{
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) _keRecord = _kh.FillKeybardEvent(vkCode);
		LOG("rec 0x" << std::hex << _keRecord._vkCode);
		return false;
	}

	if (_bEnabled == false) return false;	

	KeyboardEvent ke = _kh.FillKeybardEvent(vkCode);

	// FIXIT
	LOG(L"key pressed: " << ke.DumpStr() << L", wParam 0x" << std::hex << wParam);

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		_bHotkeyFlag = true;
	}	

	if (ke == _oh.KeyConvertLast())
	{
		return Process(ke, wParam, std::bind(&Switcher::ConvertLastWord, this, GetForegroundWindow()));
	}

	if (ke == _oh.KeyConvertSelection())
	{
		return Process(ke, wParam, std::bind(&Switcher::ConvertSelection, this, GetForegroundWindow()));
	}

	if (ke == _oh.KeySwitchToEn() && _oh.KeySwitchToEn() == _oh.KeySwitchToRu())
	{
		return Process(ke, wParam, std::bind(&Switcher::SwitchLangNext, this, GetForegroundWindow()));
	}

	if (ke == _oh.KeySwitchToEn())
	{
		return Process(ke, wParam, std::bind(&Switcher::SwitchLangEng, this, GetForegroundWindow()));
	}

	if (ke == _oh.KeySwitchToRu())
	{
		return Process(ke, wParam, std::bind(&Switcher::SwitchLangRus, this, GetForegroundWindow()));
	}

	if (ke == _oh.KeySearchInet())
	{
		return Process(ke, wParam, std::bind(&Switcher::SearchInet, this, GetForegroundWindow()));
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
			if (ke != _oh.KeySwitchToEn()) return Process(ke, wParam, std::bind(&Switcher::SwitchLangEng, this, GetForegroundWindow()));
			if (ke != _oh.KeySwitchToRu()) return Process(ke, wParam, std::bind(&Switcher::SwitchLangRus, this, GetForegroundWindow()));
		}
	}

	AppendBuffer(ke);

	return false;
}

void Switcher::AppendBuffer(KeyboardEvent& ke)
{
	// backspace
	if (ke._vkCode == VK_BACK)
	{
		if (_buffer.empty() == false) _buffer.pop_back();
		return;
	}

	// shift-home, shift-end
	if (GetLastKeyPressed() == KeyboardEvent(VK_HOME, true) || GetLastKeyPressed() == KeyboardEvent(VK_END, true))
	{
		LOG("home or end, new word");
		BeginNewWord();
	}

	// up, down, home, end
	if (ke._vkCode == VK_UP || ke._vkCode == VK_DOWN ||
		ke == KeyboardEvent(VK_END) || ke == KeyboardEvent(VK_HOME))
	{
		BeginNewWord();
		return;
	}

	// delete
	if (ke._vkCode == VK_DELETE)
	{
		// cant debug it so disabled
		BeginNewWord();
		return;
	}

	/*
	// cant debug it so disabled
	if (ke._vkCode == VK_LEFT) _cursorPos++;

	if (ke._vkCode == VK_RIGHT)
	if (_cursorPos > 0) _cursorPos--;*/

	if (ke == KeyboardEvent(VK_LEFT) ||
		ke == KeyboardEvent(VK_RIGHT))
		BeginNewWord();

	if (ke == KeyboardEvent(VK_RETURN))
	{
		LOG("VK_RETURN, new word");
		BeginNewWord();
	}

	if (ke == KeyboardEvent(VK_TAB))
	{
		LOG("VK_TAB, new word");
		BeginNewWord();
	}

	if (GetLastKeyPressed()._vkCode == VK_SPACE && !(GetLastKeyPressed().Modified()))
	{
		LOG("VK_SPACE, new word");
		BeginNewWord();
	}

	if (IsPrintable(ke._vkCode) && !ke.Modified())
	{
		_buffer.push_back(ke);
	}
}


void Switcher::ConvertLastWord(HWND hWnd)
{
	LOG("converting last word");

	if (_buffer.empty())
	{
		if (_oh.IsPlaySound()) _sh.PlayError();
		return;
	}

	DumpBuffer();

	_kh.ReleaseKey(_oh.KeyConvertLast());

	for (auto i = 0; i < _cursorPos; i++)
	{
		_kh.PressAndReleaseKey(KeyboardEvent(VK_RIGHT));
	}

	for (auto& x : _buffer)
	{
		if (IsPrintable(x._vkCode)) _kh.PressAndReleaseKey(KeyboardEvent(VK_BACK));
	}

	SwitchLangNext(hWnd);

	for (auto& x : _buffer)
	{
		if (IsPrintable(x._vkCode)) _kh.PressAndReleaseKey(x);
	}

	for (auto i = 0; i < _cursorPos; i++)
	{
		_kh.PressAndReleaseKey(KeyboardEvent(VK_LEFT));
	}

	if (_oh.IsPlaySound()) _sh.PlaySwitch();

	//_cursorPos = 0;
}

void Switcher::ConvertSelection(HWND hWnd)
{
	LOG("converting selection");

	HWND wndFocused = GetFocusedHandle(hWnd);

	_kh.ReleaseKey(_oh.KeyConvertSelection());

	wchar_t wcClassName[100];
	GetClassName(wndFocused, wcClassName, 99);

	LOG("convert selection wnd class: " << wcClassName);

	if (wcsnicmp(wcClassName, L"Edit", 4) == 0)
	{
		ConvertSelectionEdit(wndFocused);
	}
	else
	{
		ConvertSelectionOther(wndFocused);
	}

	SwitchLangNext(wndFocused);

	return;
}

void Switcher::BeginNewWord() 
{
	_buffer.clear();
	_cursorPos = 0;
}

KeyboardEvent& Switcher::GetLastKeyPressed()
{
	if (_buffer.empty()) return KeyboardEvent();
	return _buffer[_buffer.size() - 1];
}

bool Switcher::IsPrintable(DWORD vkCode) 
{
	if (vkCode >= 0x30 && vkCode <= 0x39) 
	{
		return true;
	}
	
	if (vkCode >= 0x41 && vkCode <= 0x5A) 
	{
		return true;
	}
	
	if (vkCode == VK_SPACE)
	{
		return true;
	}
	
	if (vkCode >= VK_OEM_1 && vkCode <= VK_OEM_3)
	{
		return true;
	}
	
	if (vkCode >= VK_OEM_4 && vkCode <= VK_OEM_8)
	{
		return true;
	}

	return false;
};

void Switcher::ConvertString(std::wstring& str)
{
	auto sz = wcslen(_wcRusKeys);

	for (size_t i = 0; i < str.length(); i++)
	{
		for (size_t j = 0; j < sz; j++)
		{
			if (str[i] == _wcRusKeys[j]) str[i] = _wcEngKeys[j];
			else
				if (str[i] == _wcEngKeys[j]) str[i] = _wcRusKeys[j];
		}
	}
}

void Switcher::DumpBuffer()
{
	std::wstring sDump;

	for (auto& x : _buffer)
	{
		sDump = sDump + x.ToString() + L" ";
	}

	LOG("buffer dump: " << sDump);
}

void Switcher::ConvertSelectionEdit(HWND hwndFocused)
{
	// class name == Edit
	unsigned int start = 0, next = 0;

	int txtlen = SendMessage(hwndFocused, WM_GETTEXTLENGTH, 0, 0);

	if (txtlen == 0)
	{
		if (_oh.IsPlaySound()) _sh.PlayError();
		return;
	}

	std::wstring wsFullText(txtlen + 1, ' ');

	SendMessage(hwndFocused, WM_GETTEXT, txtlen + 1, (LPARAM)wsFullText.data());
	SendMessage(hwndFocused, EM_GETSEL, (WPARAM)&start, (LPARAM)&next);

	// Get text from focused window
	std::wstring wsSelected(wsFullText.begin() + start, wsFullText.begin() + next);

	LOG("focused get text: " << wsSelected);

	// Convert text
	ConvertString(wsSelected);

	LOG("converted: " << wsSelected);

	// Send text back
	SendMessage(hwndFocused, EM_REPLACESEL, TRUE, (LPARAM)wsSelected.c_str());

	if (_oh.IsPlaySound()) _sh.PlaySwitch();

	return;
}

void Switcher::ConvertSelectionOther(HWND hWnd)
{
	if (_ch.BackupClipboard(hWnd) == false)
	{
		LOG("cant backup cb");
		_ch.DeleteBackup();
		if (_oh.IsPlaySound()) _sh.PlayError();
		return;
	}

	DWORD nCbNum = GetClipboardSequenceNumber();

	// ctrl-c
	_kh.PressAndReleaseKey(KeyboardEvent(0x43, false, true));

	// firefox workaround
	{
		_kh.PressAndReleaseKey(KeyboardEvent(0x43, false, true));
		Sleep(100);
	}	

	for (auto t = 0; t < 99; t++)
	{
		if (GetClipboardSequenceNumber() > nCbNum) break;
		Sleep(10);
	}

	if (GetClipboardSequenceNumber() == nCbNum)
	{
		LOG("cant get cb");
		_ch.RestoreClipboard(hWnd);
		if (_oh.IsPlaySound()) _sh.PlayError();
		return;
	}

	std::wstring wsCbText = _ch.GetClipboardText(hWnd);
	LOG("got cb text: " << wsCbText);

	if (wsCbText.empty())
	{
		LOG("cb text is empty");
		_ch.RestoreClipboard(hWnd);
		if (_oh.IsPlaySound()) _sh.PlayError();
		return;
	}

	// convert
	ConvertString(wsCbText);

	LOG("converted text: " << wsCbText);

	_ch.SetClipboardText(wsCbText, hWnd);

	// ctrl-v
	_kh.PressAndReleaseKey(KeyboardEvent(0x56, false, true));

	Sleep(100);

	_ch.RestoreClipboard(hWnd);

	if (_oh.IsPlaySound()) _sh.PlaySwitch();

	return;
}

HWND Switcher::GetFocusedHandle(HWND wndForeground)
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

void Switcher::SwitchLangNext(HWND hWnd)
{
	PostMessage(GetFocusedHandle(hWnd), WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, HKL_NEXT);
	Sleep(100);
}

void Switcher::SwitchLangRus(HWND hWnd)
{
	PostMessage(GetFocusedHandle(hWnd), WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)0x04190419);
}

void Switcher::SwitchLangEng(HWND hWnd)
{
	PostMessage(GetFocusedHandle(hWnd), WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)0x04090409);
}

void Switcher::SearchInet(HWND hwnd)
{
	auto str = _oh.SearchQuery();
	if (str.find(L"http://") == 0)
		str = L"http://" + str;
	ShellExecute(GetDesktopWindow(), L"open", L"http://google.com/search?q=test", NULL, NULL, SW_SHOW);
}
