#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Config config;

	try
	{
		if (!config.Load("data//config"))
		{
			ErrorLogger::Log("Failed to Load config file");
			return -1;
		}

		Window window(&config);

		int result = window.Update();

		return result;
	}
	catch (const CustomException & e)
	{
		ErrorLogger::Log(e);
	}
	catch (const std::exception& e)
	{
		ErrorLogger::Log(e);
	}
	catch (...)
	{
		ErrorLogger::Log("Unknown Exception");
	}
	return -1;
}

