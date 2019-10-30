#pragma once
#include <comdef.h>
#include <exception>
#include "StringTools.h"

//Macros

    #define EXCEPT_IF_FAILED(hr, msg) if (FAILED(hr))	throw CustomException(msg, __FILE__, __FUNCTION__, __LINE__,hr);
    #define CUSTOM_EXCEPTION(msg)						throw CustomException(msg, __FILE__, __FUNCTION__, __LINE__);
    #define LAST_EXCEPTION(msg)							throw CustomException(msg, __FILE__, __FUNCTION__, __LINE__,GetLastError());
    #define EOF_EXCEPTION(file,msg)	if(file.eof())		throw CustomException(msg, __FILE__, __FUNCTION__, __LINE__);

class CustomException
{
public:
	CustomException(const std::string& msg, const std::string& file, const std::string& function, int line, HRESULT hr = NULL);
	const char* what() const;

private:
	std::string whatmsg;
};