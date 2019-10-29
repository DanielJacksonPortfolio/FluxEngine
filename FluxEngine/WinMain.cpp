#include "Window.h"

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, int nCmdShow)
{
	Config config;

	if (!config.Load("data//config"))
	{
		return 0;
	}

	Window window;
	window.Init(hInstance, "Main Window", "Default", &config);

	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1)
	{
		return -1;
	}
	else if (gResult == 0)
	{
		return msg.wParam;
	}

	return 0;
}

