#include "ErrorLogger.h"

std::string ErrorLogger::ERROR_LOG = "";
bool ErrorLogger::showErrorLog = false;

void ErrorLogger::Log(std::string message)
{
	std::string errorMessage = "Error: " + message;
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
	//MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + message + "\n" + error.ErrorMessage();
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
	//MessageBoxA(nullptr, errorMessage.c_str(), "HRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + StringTools::WideToStandard(message) + "\n" + error.ErrorMessage();
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
	//MessageBoxA(nullptr, errorMessage.c_str(), "wHRESULT Error", MB_ICONERROR);
}

void ErrorLogger::Log(const CustomException& exception)
{
	ERROR_LOG += exception.what();
	ERROR_LOG += "\n";
	showErrorLog = true;
	//MessageBoxA(nullptr, exception.what(), "Custom Exception", MB_ICONERROR);
}

void ErrorLogger::Log(const std::exception & exception)
{
	ERROR_LOG += exception.what();
	ERROR_LOG += "\n";
	showErrorLog = true;
	//MessageBoxA(nullptr, exception.what(), "Standard Exception", MB_ICONERROR);
}
