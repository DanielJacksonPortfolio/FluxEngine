#include "ErrorLogger.h"

std::string ErrorLogger::ERROR_LOG = "";
bool ErrorLogger::showErrorLog = false;

void ErrorLogger::Log(std::string message)
{
	std::string errorMessage = "Error: " + message;
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + message + "\n" + error.ErrorMessage();
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::string errorMessage = "Error: " + StringTools::WideToStandard(message) + "\n" + error.ErrorMessage();
	ERROR_LOG += errorMessage + "\n";
	showErrorLog = true;
}

void ErrorLogger::Log(const CustomException& exception, std::string message)
{
	ERROR_LOG += exception.what();
	ERROR_LOG += ": " + message;
	ERROR_LOG += "\n";
	showErrorLog = true;
}

void ErrorLogger::Log(const std::exception & exception, std::string message)
{
	ERROR_LOG += exception.what();
	ERROR_LOG += ": " + message;
	ERROR_LOG += "\n";
	showErrorLog = true;
}
