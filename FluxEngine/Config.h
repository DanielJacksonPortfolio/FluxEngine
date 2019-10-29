#pragma once
#include <fstream>

enum class StyleType {WINDOWED,FULLSCREEN};

class Config
{
public:
	int width;
	int height;
	int displayWidth;
	int displayHeight;
	StyleType styleFlag;

	bool Load(std::string configLocation)
	{
		std::ifstream configFile(configLocation);

		if (!configFile.is_open())
		{
			return false;
		}
		int styleFlag;
		configFile >> this->width;
		configFile >> this->height;
		configFile >> styleFlag;

		this->styleFlag = static_cast<StyleType>(styleFlag);

		if (this->styleFlag == StyleType::FULLSCREEN)
		{
			this->displayWidth = GetSystemMetrics(SM_CXSCREEN);
			this->displayHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		else
		{
			this->displayWidth = this->width;
			this->displayHeight = this->height;
		}

		return true;
	}
};