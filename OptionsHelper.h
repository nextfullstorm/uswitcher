#pragma once

#include <windows.h>
#include <string>
#include "KeyboardEvent.h"

class OptionsHelper
{
public:
	OptionsHelper(std::wstring wsIniFile, std::wstring wsExeFile) : _wsIniFile(wsIniFile), _wsExeFile(wsExeFile)
	{
		LoadOptions();
	}

	~OptionsHelper()
	{

	}

	void SaveOptions()
	{
		KeToSection(L"convert_last", _keConvLast);
		KeToSection(L"convert_selection", _keConvSelect);
		KeToSection(L"switch_en", _keSwitchToEn);
		KeToSection(L"switch_ru", _keSwitchToRu);
		KeToSection(L"search_inet", _keSearchInet);

		WritePrivateProfileStringW(L"general", L"change_mouse_pointer", std::to_wstring(_bIsChangeMousePointer).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(L"general", L"flash_scrollock", std::to_wstring(_bIsFlashScrollLock).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(L"general", L"play_sound", std::to_wstring(_bIsPlaySound).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(L"general", L"change_color", std::to_wstring(_bIsChangeColor).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(L"general", L"search_query", _sSearchQuery.c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(L"general", L"track_wnd_mouse", std::to_wstring(_bTrackActiveWndByMouse).c_str(), _wsIniFile.c_str());

		if (_bIsAutostart) AddToAutostart();
		else RemoveFromAutostart();
	}

	void LoadOptions()
	{
		const unsigned char bufsz = 250;
		wchar_t wcBuf[bufsz];

		GetPrivateProfileString(L"general", L"change_mouse_pointer", L"true", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_bIsChangeMousePointer = ConvertBool(wcBuf);

		GetPrivateProfileString(L"general", L"flash_scrollock", L"true", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_bIsFlashScrollLock = ConvertBool(wcBuf);

		GetPrivateProfileString(L"general", L"play_sound", L"true", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_bIsPlaySound = ConvertBool(wcBuf);

		GetPrivateProfileString(L"general", L"change_color", L"true", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_bIsChangeColor = ConvertBool(wcBuf);

		GetPrivateProfileString(L"general", L"track_wnd_mouse", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_bTrackActiveWndByMouse = ConvertBool(wcBuf);

		GetPrivateProfileString(L"general", L"search_query", L"google.com/search?q=%s", wcBuf, bufsz - 1, _wsIniFile.c_str());
		_sSearchQuery.assign(wcBuf);

		// last
		SectionToKe(L"convert_last", _keConvLast);
		if (_keConvLast.Valid() == false)
			_keConvLast = KeyboardEvent(VK_PAUSE);

		// select
		SectionToKe(L"convert_selection", _keConvSelect);
		if (_keConvSelect.Valid() == false)
			_keConvSelect = KeyboardEvent(VK_PAUSE, true, false, false, false);

		// en
		SectionToKe(L"switch_en", _keSwitchToEn);
		if (_keSwitchToEn.Valid() == false)
			_keSwitchToEn = KeyboardEvent(0, false, true, false, true);

		// ru
		SectionToKe(L"switch_ru", _keSwitchToRu);
		if (_keSwitchToRu.Valid() == false)
			_keSwitchToRu = KeyboardEvent(0, true, false, true, false);

		// search

		SectionToKe(L"search_inet", _keSearchInet);
		if (_keSearchInet.Valid() == false)
			_keSearchInet = KeyboardEvent(VK_F1, false, false, true, true);
	}

	bool IsConvertLast()
	{
		return _keConvLast.Valid();
	}

	KeyboardEvent KeyConvertLast() const { return _keConvLast; }
	void SetKeyConvertLast(const KeyboardEvent& val) { _keConvLast = val; }

	bool IsConvertSelection()
	{
		return _keConvSelect.Valid();
	}

	KeyboardEvent KeyConvertSelection() const { return _keConvSelect; }
	void SetKeyConvertSelection(const KeyboardEvent& val) { _keConvSelect = val; }

	bool IsSwitchToRu()
	{
		return _keSwitchToRu.Valid();
	}

	KeyboardEvent KeySwitchToRu() const { return _keSwitchToRu; }
	void SetKeySwitchToRu(const KeyboardEvent& val) { _keSwitchToRu = val; }

	bool IsSwitchToEn()
	{
		return _keSwitchToEn.Valid();
	}

	KeyboardEvent KeySwitchToEn() const { return _keSwitchToEn; }
	void SetKeySwitchToEn(const KeyboardEvent& val) { _keSwitchToEn = val; }

	bool IsSearchInet()
	{
		return _keSearchInet.Valid();
	}

	KeyboardEvent KeySearchInet() const { return _keSearchInet; }
	void SetKeySearchInet(const KeyboardEvent& val) { _keSearchInet = val; }

	bool IsChangeMousePointer() const { return _bIsChangeMousePointer; }
	void SetChangeMousePointer(bool val) { _bIsChangeMousePointer = val; }

	bool IsFlashScrollLock() const { return _bIsFlashScrollLock; }
	void SetFlashScrollLock(bool val) { _bIsFlashScrollLock = val; }

	bool IsPlaySound() const { return _bIsPlaySound; }
	void SetPlaySound(bool val) { _bIsPlaySound = val; }

	bool IsChangeColor() const { return _bIsChangeColor; }
	void SetChangeColor(bool val) { _bIsChangeColor = val; }

	std::wstring SearchQuery() const { return _sSearchQuery; }
	void SetSearchQuery(const std::wstring& val) { _sSearchQuery = std::move(val); }

	bool IsTrackActiveWndByMouse() const { return _bTrackActiveWndByMouse; }
	void SetTrackActiveWndByMouse(bool val) { _bTrackActiveWndByMouse = val; }

	bool IsAutostart() const { return _bIsAutostart; }
	void SetAutostart(bool val) { _bIsAutostart = val; }

	bool IsOfficeWorkaroundEnabled() const { return true; }
	void SetOfficeWorkaroundEnabled(bool val) { ; }

private:
	inline bool ConvertBool(wchar_t* wcStr)
	{
		if (wcscmp(wcStr, L"0") == 0 ||
			wcscmp(wcStr, L"false") == 0 ||
			wcscmp(wcStr, L"no") == 0)
		{
			return false;
		}

		if (wcscmp(wcStr, L"1") == 0 ||
			wcscmp(wcStr, L"true") == 0 ||
			wcscmp(wcStr, L"yes") == 0)
		{
			return true;
		}

		return false;
	}

	inline void SectionToKe(const std::wstring& wsSection, KeyboardEvent& ke)
	{
		const unsigned char bufsz = 250;
		wchar_t wcBuf[bufsz];

		ke._vkCode = GetPrivateProfileInt(wsSection.c_str(), L"vk_code", 0, _wsIniFile.c_str());

		GetPrivateProfileString(wsSection.c_str(), L"r_shift", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bRShift = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"l_shift", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bLShift = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"r_ctrl", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bRCtrl = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"l_ctrl", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bLCtrl = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"r_alt", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bRAlt = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"l_alt", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bLAlt = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"r_win", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bRWin = ConvertBool(wcBuf);

		GetPrivateProfileString(wsSection.c_str(), L"l_win", L"false", wcBuf, bufsz - 1, _wsIniFile.c_str());
		ke._bLWin = ConvertBool(wcBuf);
	}

	inline void KeToSection(const std::wstring& wsSection, KeyboardEvent& ke)
	{
		WritePrivateProfileStringW(wsSection.c_str(), L"vk_code", std::to_wstring(ke._vkCode).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"r_shift", std::to_wstring(ke._bRShift).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"l_shift", std::to_wstring(ke._bLShift).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"r_ctrl", std::to_wstring(ke._bRCtrl).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"l_ctrl", std::to_wstring(ke._bLCtrl).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"r_alt", std::to_wstring(ke._bRAlt).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"l_alt", std::to_wstring(ke._bLAlt).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"r_win", std::to_wstring(ke._bRWin).c_str(), _wsIniFile.c_str());
		WritePrivateProfileStringW(wsSection.c_str(), L"l_win", std::to_wstring(ke._bLWin).c_str(), _wsIniFile.c_str());
	}

	void AddToAutostart()
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,
			L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
			0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
		{
			if (_wsExeFile.empty() == false)
				RegSetValueEx(hKey, L"uSwitcher", 0, REG_SZ, (BYTE *)_wsExeFile.c_str(), _wsExeFile.length() * sizeof(_wsExeFile[0]));
			RegCloseKey(hKey);
		}
	}

	void RemoveFromAutostart()
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,
			L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
			0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
		{
			RegDeleteValueW(hKey, L"uSwitcher");
			RegCloseKey(hKey);
		}
	}

	std::wstring _wsIniFile;
	std::wstring _wsExeFile;

	KeyboardEvent _keConvLast;
	KeyboardEvent _keConvSelect;
	KeyboardEvent _keSwitchToRu;
	KeyboardEvent _keSwitchToEn;
	KeyboardEvent _keSearchInet;

	bool _bIsChangeMousePointer = true;
	bool _bIsFlashScrollLock = true;
	bool _bIsPlaySound = true;
	bool _bIsChangeColor;
	std::wstring _sSearchQuery = L"google.com/search?q=%s";
	bool _bTrackActiveWndByMouse = false;
	bool _bIsAutostart = true;
};