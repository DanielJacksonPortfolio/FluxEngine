#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	EXCEPT_IF_FAILED(CoInitialize(NULL),"Failed to CoInitialize");

	Config config;

	try
	{
		if (!config.Load("data//config"))
		{
			ErrorLogger::Log("Failed to Load config file");
			return -1;
		}

		Application app;
		if (app.Init(&config))
		{
			while (app.ProcessMessages())
			{
				app.Update();
				app.RenderFrame();
			}
		}
		return 0;
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

