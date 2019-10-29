#pragma once
#include "ErrorLogger.h"
#include "Config.h"

class Window
{
public:
	bool Init(HINSTANCE hInstance, std::string windowName, std::string windowClass, Config* config);
	void RegisterWindowClass();

private:
	HINSTANCE hInstance;
	HWND hWnd;
	std::string windowClass;
	std::string windowName;
	int width;
	int height;

};

