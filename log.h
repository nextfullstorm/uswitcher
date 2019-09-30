#pragma once

#include <sstream>
#include <exception>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <iomanip>
#include <cwchar>

extern bool globalDebugFlag;
static const size_t LogBufferSize = 200;

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define WFILE WIDEN(__FILE__)
#define WFUNCTION WIDEN(__FUNCTION__)

// -- Logger
class Logger
{
public:
	static Logger& Instance()
	{
		static Logger singleton;
		return singleton;
	}

	void Log(const std::wstring& str, const std::wstring& file, const long line, const std::wstring& proc, const bool add_sourcepath)
	{
		const time_t nw = std::time(0);
		const std::tm *ltm = std::localtime(&nw);

		std::wstringstream out;
		std::swprintf(_buf, LogBufferSize - 1, L"[%02d.%02d, %02d:%02d:%02d][",
			ltm->tm_mday,
			ltm->tm_mon + 1,
			ltm->tm_hour,
			ltm->tm_min,
			ltm->tm_sec);

		out << _buf << proc << "] " << str << std::endl;

		//if (add_sourcepath) out << file << "(" << line << ")" << " : " << proc << std::endl;

		if (_fs.bad() == false)
		{
			_fs << out.str();
			_fs.flush();
		}

		OutputDebugString(out.str().c_str());

		return;
	}

	// Other non-static member functions
private:
	std::wofstream _fs;
	wchar_t _buf[LogBufferSize] = { 0 };

	Logger() 										// Private constructor
	{
		char path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), path, MAX_PATH - 1);

#ifdef LOG_START_NEW_FILE
		time_t nw = std::time(0);
		std::tm *ltm = std::localtime(&nw);

		std::sprintf(_buf, "-%02d.%02d_%02d.%02d.%02d-",
			ltm->tm_mday,
			ltm->tm_mon + 1,
			ltm->tm_hour,
			ltm->tm_min,
			ltm->tm_sec);
		std::strcat(path, _buf);
#endif

		std::strcat(path, ".log");
		_fs.open(path, std::fstream::out | std::fstream::app);
	}

	~Logger()
	{
		_fs.close();
	}

	Logger(const Logger&) = delete;					// Prevent copy-construction
	Logger& operator=(const Logger&) = delete;		// Prevent assignment
};
// -- Logger


// -- Macroses
#define LOG(strm) \
	if (globalDebugFlag)\
	{\
		std::wstringstream out; \
		out << strm;\
		Logger::Instance().Log(out.str(), WFILE, __LINE__, WFUNCTION, false);\
	}\
	else\
	{\
		std::wstringstream out; \
		out << strm << std::endl;\
		OutputDebugString(out.str().c_str());\
	}

#define LOG2(strm) \
	if (globalDebugFlag)\
	{\
		std::wstringstream out; \
		out << strm;\
		Logger::Instance().Log(out.str(), WFILE, __LINE__, WFUNCTION, true);\
	}\
	else\
	{\
		std::wstringstream out; \
		out << strm << std::endl;\
		OutputDebugString(out.str().c_str());\
	}

#define CHECK_EX(condition, strm) \
	if (/*globalDebugFlag &&*/ !(condition))\
	{ \
		if (IsDebuggerPresent()) DebugBreak();\
		std::wstringstream out; \
		out << "**********************ERROR: " << strm << std::endl << std::endl; \
		Logger::Instance().Log(out.str(), WFILE, __LINE__, WFUNCTION, true);\
		throw std::runtime_error(out.str().c_str()); \
	}

#define LEF LOG("entering " << __FUNCTION__);
#define LLF LOG("leaving " << __FUNCTION__);
// -- Macroses