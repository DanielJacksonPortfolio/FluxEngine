#pragma once
#include <fstream>
#include <iostream>
#include "../../Tools/ErrorLogger.h"

enum class StyleType {WINDOWED,FULLSCREEN};

class Config
{
public:
	int width;
	int height;
	std::string name = "";
	StyleType styleFlag;
	bool vSync;

	bool Load(std::string configLocation)
	{
		std::ifstream configFile(configLocation);

		if (!configFile.is_open())
		{
			ErrorLogger::Log("Config File failed to open");
			return false;
		}

		int styleFlag;

		try
		{
			configFile >> this->rawWidth;
			EOF_EXCEPTION(configFile, "Config File Format Incorrect : EOF Reached after width");
			configFile >> this->rawHeight;
			EOF_EXCEPTION(configFile, "Config File Format Incorrect : EOF Reached after height");
			configFile >> styleFlag;
			EOF_EXCEPTION(configFile, "Config File Format Incorrect : EOF Reached after styleFlag");
			configFile.ignore(); std::getline(configFile, this->name);
			EOF_EXCEPTION(configFile, "Config File Format Incorrect : EOF Reached after windowName");
			configFile >> vSync;
		}
		catch (CustomException & e)
		{
			ErrorLogger::Log(e);
			return false;
		}

		configFile.close();

		this->styleFlag = static_cast<StyleType>(styleFlag);

		if (this->styleFlag == StyleType::FULLSCREEN)
		{
			this->width = GetSystemMetrics(SM_CXSCREEN);
			this->height = GetSystemMetrics(SM_CYSCREEN);
		}
		else
		{
			this->width = this->rawWidth;
			this->height = this->rawHeight;
		}

		return true;
	}
private:
	int rawWidth;
	int rawHeight;
};