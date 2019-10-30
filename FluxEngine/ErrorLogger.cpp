#include "ErrorLogger.h"

void ErrorLogger::Log(std::string message)
{
	std::string errorMessage = "Error: " + message;
	MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + message + "\n" + error.ErrorMessage();
	MessageBoxA(NULL, errorMessage.c_str(), "HRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + StringTools::WideToStandard(message) + "\n" + error.ErrorMessage();
	MessageBoxA(NULL, errorMessage.c_str(), "wHRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(const CustomException& exception)
{
	MessageBoxA(NULL, exception.what(), "Custom Exception", MB_ICONERROR);
}

void ErrorLogger::Log(const std::exception & exception)
{
	MessageBoxA(NULL, exception.what(), "Standard Exception", MB_ICONERROR);
}
