#include "Window.h"

bool Window::Init(HINSTANCE hInstance, std::string windowName, std::string windowClass, Config* config)
{
	this->hInstance = hInstance;
	this->windowClass = windowClass;
	this->windowName = windowName;
	this->width = config->displayWidth;
	this->height = config->displayWidth;
	this->RegisterWindowClass();

	DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT windowRect;
	windowRect.left = GetSystemMetrics(SM_CXSCREEN) * 0.5f - this->width * 0.5f;
	windowRect.top = GetSystemMetrics(SM_CYSCREEN) * 0.5f - this->height * 0.5f;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	AdjustWindowRect(&windowRect, windowStyle, FALSE);
	this->hWnd = CreateWindowEx(
		0,
		this->windowClass.c_str(), this->windowName.c_str(),
		windowStyle,
		windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr,
		this->hInstance,
		0);

	if (this->hWnd == nullptr)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->windowName);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);

	return true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	//	return true;
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::RegisterWindowClass()
{
	WNDCLASSEXA windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = this->hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = this->windowClass.c_str();
	windowClass.hIconSm = nullptr;
	RegisterClassEx(&windowClass);
}

