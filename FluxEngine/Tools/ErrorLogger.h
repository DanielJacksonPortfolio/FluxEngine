#pragma once
#include "StrippedWindows.h"
#include "ExceptionHandler.h"

class ErrorLogger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(const CustomException& exception);
	static void Log(const std::exception& exception);
};

