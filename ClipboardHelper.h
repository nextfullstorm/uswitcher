#pragma once

#include <Windows.h>
#include <queue>
#include <crtdbg.h>
#include <tchar.h>

struct ClipBoardItem
{
	UINT format;
	LPVOID content;
	SIZE_T size;
};

class ClipboardHelper
{
public:
	ClipboardHelper()
	{

	}

	~ClipboardHelper()
	{
		DeleteBackup();
	}

	void DumpClipboard(HWND wnd)
	{
		UINT uFormat = 0;
		HGLOBAL hglb;
		LPVOID hMem;
		float totalSize = 0;

		if (!OpenClipboard(wnd))
		{
			LOG("Clipboard open error " << GetLastError());
			return;
		}

		int countFormat = CountClipboardFormats();
		LOG("Clipboard formats count " << countFormat);

		uFormat = EnumClipboardFormats(uFormat);
		while (uFormat)
		{
			hglb = GetClipboardData(uFormat);
			if (hglb != NULL)
			{
				hMem = GlobalLock(hglb);
				SIZE_T size = GlobalSize(hMem);

				LOG("Clipboard format " << uFormat << ", size " << size);

				totalSize += size;
				if (hMem != NULL) GlobalUnlock(hglb);
			}
			else
			{
				LOG("Clipboard data is NULL");
			}

			uFormat = EnumClipboardFormats(uFormat);
		}

		CloseClipboard();

		std::wstring unit = L"bytes";
		if (totalSize >= 1024)
		{
			totalSize /= 1024;
			unit = L"KB";
		}
		if (totalSize >= 1024)
		{
			totalSize /= 1024;
			unit = L"MB";
		}
		if (totalSize >= 1024)
		{
			totalSize /= 1024;
			unit = L"GB";
		}

		// FIXIT
		//LOG(L"Total size is " << totalSize << " " << unit);
	}

	bool BackupClipboard(HWND wnd)
	{
		UINT uFormat = 0;
		HGLOBAL hglb;
		LPVOID hMem;

		DeleteBackup();

		if (!OpenClipboard(wnd))
		{
			LOG("Clipboard open error " << GetLastError());
			return false;
		}

		uFormat = EnumClipboardFormats(uFormat);
		while (uFormat)
		{
			hglb = GetClipboardData(uFormat);
			if (hglb != NULL)
			{
				hMem = GlobalLock(hglb);
				SIZE_T size = GlobalSize(hMem);
				if (size > 0)
				{
					ClipBoardItem clipitem;
					clipitem.format = uFormat;
					clipitem.content = malloc(size);
					clipitem.size = size;
					memcpy(clipitem.content, hMem, size);
					_clipQueue.push(clipitem);
				}

				if (hMem != NULL) GlobalUnlock(hglb);
			}

			uFormat = EnumClipboardFormats(uFormat);
		}
		
		EmptyClipboard();
		CloseClipboard();

		return true;
	}

	bool RestoreClipboard(HWND wnd)
	{
		if (!OpenClipboard(wnd))
		{
			LOG("Clipboard open error " << GetLastError());
			return false;
		}

		EmptyClipboard();

		while (!_clipQueue.empty())
		{
			ClipBoardItem& clipitem = _clipQueue.front();
			HGLOBAL hResult = GlobalAlloc(GMEM_MOVEABLE, clipitem.size);
			if (hResult == NULL)
			{
				LOG("GlobalAlloc error " << GetLastError());
				_clipQueue.pop();
				continue;
			}
			memcpy(GlobalLock(hResult), clipitem.content, clipitem.size);
			GlobalUnlock(hResult);

			if (SetClipboardData(clipitem.format, hResult) == NULL)
			{
				LOG("SetClipboardData error " << GetLastError());
			}

			GlobalFree(hResult);
			free(clipitem.content);
			_clipQueue.pop();
		}

		CloseClipboard();

		return true;
	}

	void DeleteBackup()
	{
		while (!_clipQueue.empty())
		{
			ClipBoardItem& clipitem = _clipQueue.front();
			free(clipitem.content);
			_clipQueue.pop();
		}
	}

	std::wstring GetClipboardText(HWND wnd)
	{
		std::wstring s;

		UINT uFormat = 0;
		HGLOBAL hglb;

		LPVOID hMem;
		if (!OpenClipboard(wnd))
		{
			LOG("Clipboard open error " << GetLastError());
			return s;
		}

		hglb = GetClipboardData(CF_UNICODETEXT);
		if (hglb != NULL)
		{
			hMem = GlobalLock(hglb);
			SIZE_T size = GlobalSize(hMem);

			LOG("GetClipboardData(CF_UNICODETEXT) size " << size);

			if (size > 0) s.assign((wchar_t *)hMem, size);

			if (hMem != NULL) GlobalUnlock(hglb);
		}
		else 
		{
			hglb = GetClipboardData(CF_TEXT);
			if (hglb != NULL)
			{
				hMem = GlobalLock(hglb);
				SIZE_T size = GlobalSize(hMem);

				LOG("GetClipboardData(CF_TEXT) size " << size);

				if (size > 0) s.assign((wchar_t *)hMem, size);

				if (hMem != NULL) GlobalUnlock(hglb);
			}
			else LOG("GetClipboardData(CF_TEXT) failed " << GetLastError());
		}

		CloseClipboard();

		return s;
	}

	bool SetClipboardText(const std::wstring& str, HWND wnd)
	{
		LOG("setting cb text to " << str);

		if (str.empty()) return false;

		if (!OpenClipboard(wnd))
		{
			LOG("Clipboard open error " << GetLastError());
			return false;
		}

		EmptyClipboard();

		HGLOBAL hResult = GlobalAlloc(GMEM_MOVEABLE, str.length()*sizeof(str[0]));
		if (hResult == NULL)
		{
			LOG("GlobalAlloc error " << GetLastError());
			CloseClipboard();
			return false;
		}

		memcpy(GlobalLock(hResult), str.data(), str.length()*sizeof(str[0]));
		GlobalUnlock(hResult);

		if (SetClipboardData(CF_UNICODETEXT, hResult) == NULL)
		{
			LOG("SetClipboardData error " << GetLastError());
		}

		GlobalFree(hResult);
		CloseClipboard();

		return true;
	}

private:
	std::queue<ClipBoardItem> _clipQueue;
};


