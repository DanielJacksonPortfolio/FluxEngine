#pragma once
#include "StrippedWindows.h"
#include "ExceptionHandler.h"

class ErrorLogger
{
public:
	static std::string ERROR_LOG;
	static bool showErrorLog;

	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(const CustomException& exception, std::string message = "");
	static void Log(const std::exception& exception, std::string message = "");
};

