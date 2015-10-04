#pragma once

#include <windows.h>
#include <string>
#include <mmsystem.h>
#pragma comment (lib, "Winmm.lib")

class SoundHelper
{

public:
	SoundHelper(std::wstring sFileError, std::wstring sFileSwitch) : _sFileError(sFileError), _sFileSwitch(sFileSwitch)
	{

	}

	~SoundHelper() {}

	void PlayError()
	{
		PlaySoundW(_sFileError.c_str(), NULL, SND_ASYNC | SND_FILENAME | SND_SYSTEM | SND_NODEFAULT);
	}

	void PlaySwitch()
	{
		PlaySoundW(_sFileSwitch.c_str(), NULL, SND_ASYNC | SND_FILENAME | SND_SYSTEM | SND_NODEFAULT);
	}

protected:
private:
	std::wstring _sFileError;
	std::wstring _sFileSwitch;
};