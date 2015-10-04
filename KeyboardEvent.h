#pragma once
#include <windows.h>
#include <sstream>
#include "VkCodes.h"
#include "log.h"


struct KeyboardEvent
{
	KeyboardEvent(DWORD vkCode, bool bRShift, bool bLShift, bool bRCtrl, bool bLCtrl) : _vkCode(vkCode), 
		_bRShift(bRShift), _bLShift(bLShift), _bRCtrl(bRCtrl), _bLCtrl(bLCtrl)
	{
		_bRAlt = false;
		_bLAlt = false;
		_bRWin = false;
		_bLWin = false;
	}

	KeyboardEvent(DWORD vkCode, bool bShift, bool bCtrl) : _vkCode(vkCode), _bRShift(bShift), _bLShift(bShift), 
		_bRCtrl(bCtrl), _bLCtrl(bCtrl)
	{
		_bRAlt = false;
		_bLAlt = false;
		_bRWin = false;
		_bLWin = false;
	}

	KeyboardEvent(DWORD vkCode, bool bShift) : _vkCode(vkCode), _bRShift(bShift), _bLShift(bShift)
	{
		_bRCtrl = false;
		_bLCtrl = false;
		_bRAlt = false;
		_bLAlt = false;
		_bRWin = false;
		_bLWin = false;
	}

	KeyboardEvent(DWORD vkCode) : _vkCode(vkCode)
	{
		_bRShift = false;
		_bLShift = false;
		_bRCtrl = false;
		_bLCtrl = false;
		_bRAlt = false;
		_bLAlt = false;
		_bRWin = false;
		_bLWin = false;
	}

	KeyboardEvent()
	{
		_vkCode = 0;
		_bRShift = false;
		_bLShift = false;
		_bRCtrl = false;
		_bLCtrl = false;
		_bRAlt = false;
		_bLAlt = false;
		_bRWin = false;
		_bLWin = false;
	}

	bool Valid()
	{
		return !((*this) == KeyboardEvent());
	}

	bool Modified()
	{
		if (/*_bRShift ||
			_bLShift ||*/
			_bRCtrl ||
			_bLCtrl ||
			_bRAlt ||
			_bLAlt ||
			_bRWin ||
			_bLWin)
		{
			return true;
		}

		return false;
	}

	size_t KeyCount()
	{
		if (Valid() == false) return 0;

		size_t cnt = 0;
		if (_vkCode) cnt++;
		if (_bRShift) cnt++;
		if (_bLShift) cnt++;
		if (_bRCtrl) cnt++;
		if (_bLCtrl) cnt++;
		if (_bRAlt) cnt++;
		if (_bLAlt) cnt++;
		if (_bRWin) cnt++;
		if (_bLWin) cnt++;

		return cnt;
	}

	void Dump()
	{
		LOG("code " << _vkCode << ", lalt " << _bLAlt << ", ralt " << _bRAlt << ", lctrl " << _bLCtrl << ", rctrl " << _bRCtrl);
	}

	std::wstring DumpStr()
	{
		std::wostringstream res;
		res << "code 0x" << std::hex << _vkCode << ", lalt " << _bLAlt << ", ralt " << _bRAlt << ", lctrl " <<
			_bLCtrl << ", rctrl " << _bRCtrl << ", lshift " << _bLShift << ", rshift " << _bRShift;
		return res.str();
	}

	std::wstring ToString()
	{
		std::wstring res;
		if (_bRAlt) res = res + std::wstring(vkcodes[VK_RMENU]) + L" + ";
		if (_bLAlt) res = res + std::wstring(vkcodes[VK_LMENU]) + L" + ";
		if (_bRCtrl) res = res + std::wstring(vkcodes[VK_RCONTROL]) + L" + ";
		if (_bLCtrl) res = res + std::wstring(vkcodes[VK_LCONTROL]) + L" + ";
		if (_bLShift) res = res + std::wstring(vkcodes[VK_LSHIFT]) + L" + ";
		if (_bRShift) res = res + std::wstring(vkcodes[VK_RSHIFT]) + L" + ";
		if (_bRWin) res = res + std::wstring(vkcodes[VK_RWIN]) + L" + ";
		if (_bLWin) res = res + std::wstring(vkcodes[VK_LWIN]) + L" + ";
		if (_vkCode) res = res + std::wstring(vkcodes[_vkCode]) + L" + ";
		res.erase(res.rfind(L" + "));

		return res;
	}

	bool operator==(const KeyboardEvent &ke) const 
	{
		if (ke._vkCode == _vkCode &&
			ke._bRAlt == _bRAlt &&
			ke._bLAlt == _bLAlt &&
			ke._bRCtrl == _bRCtrl &&
			ke._bLCtrl == _bLCtrl &&
			ke._bRShift == _bRShift &&
			ke._bLShift == _bLShift &&
			ke._bRWin == _bRWin &&
			ke._bLWin == _bLWin)
			return true;

		return false;
	}

	bool operator!=(const KeyboardEvent &ke) const
	{
		return !((*this) == ke);
	}

	DWORD _vkCode;

	bool _bRShift;
	bool _bLShift;

	bool _bRCtrl;
	bool _bLCtrl;

	bool _bRWin;
	bool _bLWin;

	bool _bRAlt;
	bool _bLAlt;

};