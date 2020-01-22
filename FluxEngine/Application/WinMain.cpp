#include "../Application/Application.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//EXCEPT_IF_FAILED(CoInitialize(nullptr),"Failed to CoInitialize");
	HRESULT hr = CoInitialize(nullptr);

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

