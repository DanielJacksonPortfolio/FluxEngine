#include "ErrorLogger.h"

void ErrorLogger::Log(std::string message)
{
	std::string errorMessage = "Error: " + message;
	MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + message + "\n" + error.ErrorMessage();
	MessageBoxA(nullptr, errorMessage.c_str(), "HRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + StringTools::WideToStandard(message) + "\n" + error.ErrorMessage();
	MessageBoxA(nullptr, errorMessage.c_str(), "wHRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(const CustomException& exception)
{
	MessageBoxA(nullptr, exception.what(), "Custom Exception", MB_ICONERROR);
}

void ErrorLogger::Log(const std::exception & exception)
{
	MessageBoxA(nullptr, exception.what(), "Standard Exception", MB_ICONERROR);
}
