#pragma once

#include <sstream>
#include <exception>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <iomanip>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define WFILE WIDEN(__FILE__)
#define WFUNCTION WIDEN(__FUNCTION__)

inline void log_function(const std::wstring& str, const std::wstring& file, long line, const std::wstring& proc, bool add_sourcepath)
{
	time_t nw = std::time(0);
	std::tm *ltm = std::localtime(&nw);

	std::wstringstream out;
	std::wofstream fs;

	out /*<< std::endl*/ << "[" << std::setfill(L'0') << std::setw(2) << ltm->tm_mday \
		<< "." << std::setfill(L'0') << std::setw(2) << ltm->tm_mon + 1 << \
		", "  << std::setfill(L'0') << std::setw(2) << ltm->tm_hour \
		<< ":" << std::setfill(L'0') << std::setw(2) << ltm->tm_min \
		<< ":" << std::setfill(L'0') << std::setw(2) << ltm->tm_sec \
		<< "] " << str << std::endl;

	if (add_sourcepath) out << file << "(" << line << ")" << " : " << proc << std::endl;

	fs.open("uswitcher.log", std::fstream::out | std::fstream::app);
	if (fs.bad() == false) fs << out.str();
	fs.close();

	OutputDebugString(out.str().c_str());

	return;
}

#define LOG(strm) \
	if (true)\
	{\
		std::wstringstream out; \
		out << strm;\
		log_function(out.str(), WFILE, __LINE__, WFUNCTION, false);\
	}\
	else\
	{\
		std::wstringstream out; \
		out << strm << std::endl;\
		OutputDebugString(out.str().c_str());\
	}

#define LOG2(strm) \
	if (true)\
	{\
		std::stringstream out; \
		out << strm;\
		log_function(out.str(), __FILE__, __LINE__, __FUNCTION__, true);\
	}\
	else\
	{\
		std::stringstream out; \
		out << strm << std::endl;\
		OutputDebugStringA(out.str().c_str());\
	}

#define CHECK_EX(condition, strm) \
	if (/*globalDebugFlag &&*/ !(condition))\
	{ \
		if (IsDebuggerPresent()) DebugBreak();\
		std::stringstream out; \
		out << "**********************ERROR: " << strm << std::endl << std::endl; \
		log_function(out.str(), __FILE__, __LINE__, __FUNCTION__, true);\
		throw std::runtime_error(out.str().c_str()); \
	}

#define LEF LOG("entering " << __FUNCTION__);
#define LLF LOG("leaving " << __FUNCTION__);