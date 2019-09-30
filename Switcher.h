#pragma once

#include <windows.h>
#include <deque>
#include <string>
#include <functional>
#include <thread>
#include "KeyboardHelper.h"
#include "OptionsHelper.h"
#include "log.h"
#include "SoundHelper.h"
#include "SelectionConverter.h"
#include "LastWordConverter.h"
#include "Layouts.h"
#include <map>

using LayoutChangedCallback = std::function<void(const HKL&)>;

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
	void RecordModeOn() { _bRecordMode = true; _keRecord = KeyboardEvent(); }
	void RecordModeOff() { _bRecordMode = false; }
	KeyboardEvent GetRecordedEvent() { return _keRecord; }
	void HotkeyComboBreaker() { _bHotkeyFlag = false; }
	HKL GetCurrentLayout() { return _WndLayout; }
	LayoutChangedCallback _layoutChangedCallback = nullptr;

	void SetLayoutChangedCallback(const LayoutChangedCallback& cb)
	{
		_layoutChangedCallback = cb;
	}

	void Enable()
	{
		_bEnabled = true;
		_WndTrackerLoop = std::thread(&Switcher::WndTrackerProc, this);
	}

	void Disable()
	{
		_bEnabled = false;
		if (_WndTrackerLoop.joinable()) _WndTrackerLoop.join();
	}

	void WndTrackerProc()
	{
		while (_bEnabled)
		{
			POINT p;
			HWND wnd;

			std::this_thread::sleep_for(std::chrono::milliseconds(250));

			if (_oh.IsTrackActiveWndByMouse())
			{
				GetCursorPos(&p);
				wnd = WindowFromPoint(p);
			}
			else wnd = GetForegroundWindow();

			if (!IsWindow(wnd)) continue;

			auto pid = GetWindowThreadProcessId(wnd, nullptr);
			auto newLayout = GetKeyboardLayout(pid);
			SetWndLayoutValue(newLayout);
		}
	}

	// returns true if event processed and no need to call next hook
	bool ProcessKeyPress(const DWORD vkCode, const WPARAM wParam, const LPARAM lParam)
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
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::SwitchLangNext, this, std::placeholders::_1));
		}

		if (ke == _oh.KeySwitchToEn())
		{
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::SwitchLangEng, this, std::placeholders::_1));
		}

		if (ke == _oh.KeySwitchToRu())
		{
			return ProcessHotkey(ke, wParam, std::bind(&Switcher::SwitchLangRus, this, std::placeholders::_1));
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
			auto* pKhs = (KBDLLHOOKSTRUCT*)lParam;

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
			auto* pKhs = (KBDLLHOOKSTRUCT*)lParam;

			if (ke != _oh.KeySwitchToEn() || ke != _oh.KeySwitchToRu()) return false;

			if ((pKhs->flags & LLKHF_INJECTED) != LLKHF_INJECTED)
			{
				if (ke != _oh.KeySwitchToEn()) return ProcessHotkey(ke, wParam, std::bind(&Switcher::SwitchLangEng, this, std::placeholders::_1));
				if (ke != _oh.KeySwitchToRu()) return ProcessHotkey(ke, wParam, std::bind(&Switcher::SwitchLangRus, this, std::placeholders::_1));
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
	std::thread _WndTrackerLoop;
	HKL _WndLayout = nullptr;
	std::vector<std::wstring> _OfficeBlacklist = { 
		L"OpusApp", L"XLMAIN", L"PPTFrameClass", L"VISIOA", L"HwndWrapper[DefaultDomain" };

	void SetWndLayoutValue(const HKL newLayout)
	{
		const auto backup = _WndLayout;
		_WndLayout = newLayout;

		if (_layoutChangedCallback != nullptr && backup != newLayout) _layoutChangedCallback(_WndLayout);
	}

	bool ProcessHotkey(const KeyboardEvent& ke, const WPARAM wParam, const std::function<void(HWND)>& fWorker)
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

		auto hwnd = GetForegroundWindow();
		if (IsWindow(hwnd)) fWorker(hwnd);

		_bHotkeyFlag = false;

		if (ke.Modified() && ke._vkCode == 0) return false;

		return true;
	}

	void ConvertLastWord(const HWND hWnd)
	{
		LOG("converting last word");

		HWND wndFocused = GetFocusedHandle(hWnd);

		_kh.UnpressKey(_oh.KeyConvertLast());

		if (_lwc.ConvertLastWord(wndFocused, std::bind(&Switcher::SwitchLangNext, this, std::placeholders::_1)) == false)
		{
			if (_oh.IsPlaySound()) _sh.PlayError();
		}
		else
		{
			if (_oh.IsPlaySound()) _sh.PlaySwitch();
		}
	}

	void ConvertSelection(const HWND hWnd)
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
	}

	HWND GetFocusedHandle(const HWND wndForeground)
	{
		int  idActive = GetWindowThreadProcessId(wndForeground, nullptr);

		// вот тут https://social.msdn.microsoft.com/Forums/windowsserver/en-US/ee25af6a-e109-4500-bc2e-24fcc4838b0e/getfocus-returns-null-for-ms-word-and-may-be-in-other-office-apps?forum=vcgeneral
		// советуют не использовать AttachThreadInput попусту, а советуют GetGUIThreadInfo

		GUITHREADINFO gti = { 0 };
		gti.cbSize = sizeof(GUITHREADINFO);
		if (GetGUIThreadInfo(idActive, &gti) == NULL) return wndForeground;

		if (IsWindow(gti.hwndFocus) == FALSE)
			return wndForeground;

		return gti.hwndFocus;
	}

	void SearchInet(const HWND hwnd)
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

		ShellExecute(GetDesktopWindow(), L"open", Query.c_str(), nullptr, nullptr, SW_SHOW);

		_lwc.BeginNewWord();
	}

	std::wstring GetLayoutFriendlyName(const HKL layout)
	{
		if (layout == LAYOUT_EN) return L"EN";
		else if (layout == LAYOUT_RU) return L"RU";
		else return L"unknown";
	}

	void SwitchLang(const HWND hWndFocused, const HKL layout)
	{
		LOG("switching_v2 to " << GetLayoutFriendlyName(layout) << " from " << GetLayoutFriendlyName(_WndLayout));

		if (_WndLayout != layout)
		{
			KeyboardEvent ke;
			ke._vkCode = VK_SPACE;
			ke._bLWin = true;
			_kh.PressAndReleaseKey(ke);
		}
		
		/*
		if (_oh.IsOfficeWorkaroundEnabled())
		{
			auto parentWnd = FindParent(hWndFocused);

			wchar_t className[128] = { 0 };
			GetClassName(parentWnd, className, 128 - 1);
			std::wstring clName(className);

			LOG("office workaround: class name " << className);

			for (const auto& x : _OfficeBlacklist)
			{
				if (string_starts_with(clName, x))
				{
					if (_WndLayout != layout)
					{
						KeyboardEvent ke;
						ke._vkCode = VK_SPACE;
						ke._bLWin = true;
						_kh.PressAndReleaseKey(ke);
					}

					return;
				}
			}
		}

		LOG("classic switch");
		if (layout != nullptr) SetWndLayoutValue(layout);
		PostMessage(hWndFocused, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(layout));*/
	}

	// hWndFocused - foreground window
	void SwitchLangNext(const HWND hWndFocused)
	{
		SwitchLang(hWndFocused, nullptr);
		Sleep(100);
	}

	// hWndFocused - foreground window
	void SwitchLangRus(const HWND hWndFocused)
	{
		SwitchLang(hWndFocused, LAYOUT_RU);
	}

	// hWndFocused - foreground window
	void SwitchLangEng(const HWND hWndFocused)
	{
		SwitchLang(hWndFocused, LAYOUT_EN);
	}

	static HWND FindParent(HWND h)
	{
		auto tmp = GetParent(h);
		if (tmp == NULL) return h;

		while (true)
		{
			auto parent = GetParent(tmp);
			if (parent != NULL) tmp = parent;
			else return tmp;
		}
	}

	static bool string_starts_with(std::wstring const & value, std::wstring const & starts)
	{
		if (starts.size() > value.size()) return false;
		return std::equal(starts.begin(), starts.end(), value.begin());
	}
};
