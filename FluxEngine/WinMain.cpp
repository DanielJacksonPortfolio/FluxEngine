#include <Windows.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,int nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);

	//INIT

	return 0;
}	
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
//{
//	HRESULT hr = CoInitialize(NULL);
//	if (FAILED(hr))
//	{
//		ErrorLogger::Log(hr, "Failed to Co-Initialize");
//		return -1;
//	}
//
//	Engine engine;
//	if (engine.Init(hInstance, "DirectX Engine", "MyWindowClass", 1600, 900))
//	{
//		while (engine.ProcessMessages())
//		{
//			engine.Update();
//			engine.RenderFrame();
//		}
//	}
//	return 0;
//}