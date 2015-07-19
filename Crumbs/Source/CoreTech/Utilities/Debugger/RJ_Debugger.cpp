#include "RJ_Debugger.h"
#include <iostream>
#include "RJ_ConsoleColor.h"

RJ_Debugger RJ_Debugger::m_Instance;

RJ_Debugger::RJ_Debugger(void)
{
	m_Threshold = RJ_NOTSET;
}

void RJ_Debugger::Error_t(const RJ_ERROR_TYPE _error, const char* _message, 
		const char* _file, const char* _function, const int _line)
{
	#if defined(DEBUG) | defined(_DEBUG)
		if(_error <= m_Threshold)
			ConsoleError_t(_error,_message,_file,_function,_line);
	#endif
}

void RJ_Debugger::ConsoleError_t(const RJ_ERROR_TYPE _error, const char* _message,
		const char* _file, const char* _function, const int _line)
{
	SetErrorColor(_error);

	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "%s - %s %s %s() Line: %d : %s\n\n", 
		GetErrorType(_error), __TIMESTAMP__, _file, _function, _line, _message);

	std::cout << buffer << DEFAULT;
}

void RJ_Debugger::Log(const char* _message)
{
	#if defined(DEBUG) | defined(_DEBUG)
		std::cout << _message;
	#endif
}

char* RJ_Debugger::GetErrorType(const RJ_ERROR_TYPE _error)
{
	switch(_error)
	{
		case RJ_FATAL:
		{
			return "FATAL";
		} break;

		case RJ_ALERT:
		{
			return "ALERT";
		} break;

		case RJ_CRIT:
		{
			return "CRITICAL";
		} break;

		case RJ_ERROR:
		{
			return "ERROR";
		} break;

		case RJ_WARN:
		{
			return "WARNING";
		} break;

		case RJ_NOTICE:
		{
			return "NOTICE";
		} break;

		case RJ_INFO:
		{
			return "INFO";
		} break;

		case RJ_DEBUG:
		{
			return "DEBUG";
		} break;

		case RJ_NOTSET:
		{
			return "NOTSET";
		} break;

		default:
		{
			return "UNKNOWN";
		} break;
	}
}

void RJ_Debugger::SetErrorColor(const RJ_ERROR_TYPE _error)
{
	switch(_error)
	{
		case RJ_FATAL:
		{
			std::cout << RED;
		} break;

		case RJ_ALERT:
		{
			std::cout << D_YELLOW;
		} break;

		case RJ_CRIT:
		{
			std::cout << YELLOW;
		} break;

		case RJ_ERROR:
		{
			std::cout << MAGENTA;
		} break;

		case RJ_WARN:
		{
			std::cout << D_BLUE;
		} break;

		case RJ_NOTICE:
		{
			std::cout << BLUE;
		} break;

		case RJ_INFO:
		{
			std::cout << D_GREEN;
		} break;

		case RJ_DEBUG:
		{
			std::cout << GREEN;
		} break;

		default:
		{
			std::cout << DEFAULT;
		} break;
	}
}