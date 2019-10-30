#pragma once
#include <fstream>
#include <iostream>

enum class StyleType {WINDOWED,FULLSCREEN};

class Config
{
public:
	int width;
	int height;
	std::string name= "BOB";
	StyleType styleFlag;

	bool Load(std::string configLocation)
	{
		std::ifstream configFile(configLocation);

		if (!configFile.is_open())
		{
			return false;
		}
		int styleFlag;
		configFile >> this->rawWidth;
		configFile >> this->rawHeight;
		configFile >> styleFlag;
		configFile.ignore();
		std::getline(configFile, this->name);
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