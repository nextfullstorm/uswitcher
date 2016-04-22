#pragma once
#include <windows.h>
#include "log.h"
#include "ClipboardHelper.h"
#include "KeyboardHelper.h"

class SelectionConverter
{
public:
	SelectionConverter::SelectionConverter()
	{
	}

	SelectionConverter::~SelectionConverter()
	{
	}

	// true if success, false on error (to play right sound)
	bool ConvertSelection(HWND wndFocused)
	{
		if (IsClassNameEdit(wndFocused))
			return ConvertSelectionEdit(wndFocused);
		else
			return ConvertSelectionOther(wndFocused);

		return false;
	}

	std::wstring GetSelectionText(HWND wndFocused)
	{
		if (IsClassNameEdit(wndFocused))
			return GetSelectedTextEdit(wndFocused);
		else
			return GetSelectedTextOther(wndFocused);
	}

private:
	ClipboardHelper _ch;
	KeyboardHelper _kh;
	const wchar_t* _wcRusKeys = L"¨ÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ¸éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáş\0";
	const wchar_t* _wcEngKeys = L"~QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>`qwertyuiop[]asdfghjkl;'zxcvbnm,.\0";

	bool IsClassNameEdit(HWND wndFocused)
	{
		wchar_t wcClassName[100];
		GetClassName(wndFocused, wcClassName, 99);

		if (wcsnicmp(wcClassName, L"Edit", 4) == 0)
			return true;

		return false;
	}

	void ConvertString(std::wstring& str)
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

	std::wstring GetSelectedTextEdit(HWND hwndFocused)
	{
		// class name == Edit
		unsigned int start = 0, next = 0;

		int txtlen = SendMessage(hwndFocused, WM_GETTEXTLENGTH, 0, 0);

		if (txtlen == 0)
		{
			//if (_oh.IsPlaySound()) _sh.PlayError();
			return false;
		}

		std::wstring wsFullText(txtlen + 1, ' ');

		SendMessage(hwndFocused, WM_GETTEXT, txtlen + 1, (LPARAM)wsFullText.data());
		SendMessage(hwndFocused, EM_GETSEL, (WPARAM)&start, (LPARAM)&next);

		// Get text from focused window
		return std::wstring(wsFullText.begin() + start, wsFullText.begin() + next);
	}

	bool ConvertSelectionEdit(HWND hwndFocused)
	{
		std::wstring wsSelected = GetSelectedTextEdit(hwndFocused);

		LOG("got selected text (editbox): " << wsSelected);

		// Convert text
		ConvertString(wsSelected);

		LOG("converted: " << wsSelected);

		// Send text back
		SendMessage(hwndFocused, EM_REPLACESEL, TRUE, (LPARAM)wsSelected.c_str());

		return true;
	}

	std::wstring GetSelectedTextOther(HWND hwndFocused)
	{
		if (_ch.BackupClipboard(hwndFocused) == false)
		{
			LOG("cant backup cb");
			_ch.DeleteBackup();
			return std::wstring();
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
			_ch.RestoreClipboard(hwndFocused);
			return std::wstring();
		}

		return _ch.GetClipboardText(hwndFocused);
	}

	bool ConvertSelectionOther(HWND hwndFocused)
	{
		std::wstring wsCbText = GetSelectedTextOther(hwndFocused);
		LOG("got selected text (via cb): " << wsCbText);

		if (wsCbText.empty())
		{
			LOG("cb text is empty");
			_ch.RestoreClipboard(hwndFocused);
			return false;
		}

		// convert
		ConvertString(wsCbText);

		LOG("converted text: " << wsCbText);

		_ch.SetClipboardText(wsCbText, hwndFocused);

		// ctrl-v
		_kh.PressAndReleaseKey(KeyboardEvent(0x56, false, true));

		Sleep(100);

		_ch.RestoreClipboard(hwndFocused);

		return true;
	}
};

