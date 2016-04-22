#pragma once
#include "KeyboardEvent.h"
#include <deque>
#include "KeyboardHelper.h"
#include "SoundHelper.h"
#include "SwitchLang.h"


class LastWordConverter
{

public:
	LastWordConverter() {}
	~LastWordConverter() {}

	void AppendBuffer(KeyboardEvent& ke)
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

	bool ConvertLastWord(HWND wndFocused)
	{
		if (_buffer.empty()) return false;

		DumpBuffer();

		for (auto i = 0; i < _cursorPos; i++)
		{
			_kh.PressAndReleaseKey(KeyboardEvent(VK_RIGHT));
		}

		for (auto& x : _buffer)
		{
			if (IsPrintable(x._vkCode)) _kh.PressAndReleaseKey(KeyboardEvent(VK_BACK));
		}

		SwitchLangNext(wndFocused);

		for (auto& x : _buffer)
		{
			if (IsPrintable(x._vkCode)) _kh.PressAndReleaseKey(x);
		}

		for (auto i = 0; i < _cursorPos; i++)
		{
			_kh.PressAndReleaseKey(KeyboardEvent(VK_LEFT));
		}

		return true;
	}

	void BeginNewWord()
	{
		_buffer.clear();
		_cursorPos = 0;
	}


protected:
private:
	std::deque<KeyboardEvent> _buffer;
	KeyboardHelper _kh;
	int _cursorPos = 0;

	bool IsPrintable(DWORD vkCode)
	{
		if ((vkCode >= 0x30 && vkCode <= 0x39) ||
			(vkCode >= 0x41 && vkCode <= 0x5A) ||
			(vkCode >= VK_OEM_1 && vkCode <= VK_OEM_3) ||
			(vkCode >= VK_OEM_4 && vkCode <= VK_OEM_8) ||
			(vkCode == VK_SPACE))
		{
			return true;
		}

		return false;
	};

	void DumpBuffer()
	{
		std::wstring sDump;

		for (auto& x : _buffer)
		{
			sDump = sDump + x.ToString() + L" ";
		}

		LOG("buffer dump: " << sDump);
	}

	KeyboardEvent GetLastKeyPressed()
	{
		if (_buffer.empty()) return KeyboardEvent();
		return _buffer[_buffer.size() - 1];
	}

};