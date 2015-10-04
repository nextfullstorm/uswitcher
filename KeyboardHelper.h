#pragma once

#include <windows.h>
#include <vector>
#include "KeyboardEvent.h"
#include "log.h"

class KeyboardHelper
{
public:
	KeyboardHelper()
	{

	}

	~KeyboardHelper()
	{

	}

	void PressAndReleaseKey(KeyboardEvent& ke)
	{
		if (ke._bRShift)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RSHIFT;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bLShift)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LSHIFT;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bRCtrl)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}
		
		if (ke._bLCtrl)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bRWin)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bLWin)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bRAlt)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._bLAlt)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = 0;
			_vinp.push_back(inp);
		}

		if (ke._vkCode)
		{
			INPUT inpDown;
			inpDown.ki.dwFlags = 0;
			inpDown.type = INPUT_KEYBOARD;
			inpDown.ki.wVk = ke._vkCode;
			inpDown.ki.wScan = 0;
			inpDown.ki.dwFlags = 0;
			_vinp.push_back(inpDown);
			//////////////////////////////////////////////////////////////////////////
			INPUT inpUp;
			inpUp.ki.dwFlags = 0;
			inpUp.type = INPUT_KEYBOARD;
			inpUp.ki.wVk = ke._vkCode;
			inpUp.ki.wScan = 0;
			inpUp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inpUp);
		}		

		if (ke._bRShift)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RSHIFT;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLShift)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LSHIFT;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRCtrl)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLCtrl)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRWin)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLWin)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRAlt)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLAlt)
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (_vinp.empty() == false) SendInput(_vinp.size(), _vinp.data(), sizeof(INPUT));

		_vinp.clear();
	}

	void ReleaseKey(KeyboardEvent& ke)
	{
		if (ke._vkCode != VK_RSHIFT && 
			ke._vkCode != VK_LSHIFT &&
			ke._vkCode != VK_LCONTROL &&
			ke._vkCode != VK_RCONTROL &&
			ke._vkCode != VK_LMENU &&
			ke._vkCode != VK_RMENU &&
			ke._vkCode != VK_LWIN &&
			ke._vkCode != VK_RWIN &&
			ke._vkCode != 0)
		{
			INPUT inpUp;
			inpUp.ki.dwFlags = 0;
			inpUp.type = INPUT_KEYBOARD;
			inpUp.ki.wVk = ke._vkCode;
			inpUp.ki.wScan = 0;
			inpUp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inpUp);
		}		

		if (ke._bRShift && KeyPressed(VK_RSHIFT))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RSHIFT;
			
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLShift && KeyPressed(VK_LSHIFT))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LSHIFT;

			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRCtrl && KeyPressed(VK_RCONTROL))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLCtrl && KeyPressed(VK_LCONTROL))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LCONTROL;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRWin && KeyPressed(VK_RWIN))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLWin && KeyPressed(VK_LWIN))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LWIN;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bRAlt && KeyPressed(VK_RMENU))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_RMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (ke._bLAlt && KeyPressed(VK_LMENU))
		{
			INPUT inp;
			inp.ki.dwFlags = 0;
			inp.type = INPUT_KEYBOARD;
			inp.ki.wVk = VK_LMENU;
			inp.ki.wScan = 0;
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			_vinp.push_back(inp);
		}

		if (_vinp.empty() == false)
		{
			if (SendInput(_vinp.size(), _vinp.data(), sizeof(INPUT)) == 0)
				LOG("releasing key 0x" << std::hex << ke._vkCode << " failed with " << GetLastError());

			_vinp.clear();

			LOG("releasing key 0x" << std::hex << ke._vkCode);

			return;
		}

		LOG("empty keycode in ReleaseKey");
	}

	void FlashScrollLockLed(bool bEnableLed)
	{
		bool ScrollLock = KeyToggled(VK_SCROLL);

		if (bEnableLed && !ScrollLock)
		{
			LOG("scroll on");
			PressAndReleaseKey(KeyboardEvent(VK_SCROLL));
		}

		if (!bEnableLed && ScrollLock)
		{
			LOG("scroll off");
			PressAndReleaseKey(KeyboardEvent(VK_SCROLL));
		}
	}

	bool KeyPressed(DWORD vkCode)
	{
		return ((GetAsyncKeyState(vkCode) & 0x8000));
	}

	bool KeyToggled(DWORD vkCode)
	{
		return ((GetKeyState(vkCode) & 0x0001));
	}

	KeyboardEvent FillKeybardEvent(DWORD vkCode)
	{
		KeyboardEvent ke;
		ke._vkCode = vkCode;
		ke._bRAlt = KeyPressed(VK_RMENU);
		ke._bLAlt = KeyPressed(VK_LMENU);
		ke._bRCtrl = KeyPressed(VK_RCONTROL);
		ke._bLCtrl = KeyPressed(VK_LCONTROL);
		ke._bRShift = KeyPressed(VK_RSHIFT);
		ke._bLShift = KeyPressed(VK_LSHIFT);
		ke._bRWin = KeyPressed(VK_RWIN);
		ke._bLWin = KeyPressed(VK_LWIN);

		if (vkCode == VK_RMENU)
		{
			ke._bRAlt = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_LMENU)
		{
			ke._bLAlt = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_RCONTROL)
		{
			ke._bRCtrl = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_LCONTROL)
		{
			ke._bLCtrl = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_RSHIFT)
		{
			ke._bRShift = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_LSHIFT)
		{
			ke._bLShift = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_RWIN)
		{
			ke._bRWin = true;
			ke._vkCode = 0;
		}

		if (vkCode == VK_LWIN)
		{
			ke._bLWin = true;
			ke._vkCode = 0;
		}

		return ke;
	}

private:
	std::vector<INPUT> _vinp;
};