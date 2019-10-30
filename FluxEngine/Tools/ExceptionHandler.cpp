#include "ExceptionHandler.h"

CustomException::CustomException(const std::string& msg, const std::string& file, const std::string& function, int line, HRESULT hr)
{
	whatmsg = "Messsage: " + std::string(msg) + "\n";
	if (hr != NULL)
	{
		_com_error error(hr);
		whatmsg += error.ErrorMessage();
	}
	whatmsg += "\nFile: " + std::string(file);
	whatmsg += "\nFunction: " + std::string(function);
	whatmsg += "\nLine: " + std::to_string(line);
}

const char* CustomException::what() const
{
	return whatmsg.c_str();
}