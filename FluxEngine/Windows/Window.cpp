#include "../Game/Application.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() : hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEXA windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = HandleMessageSetup;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetInstance();
	windowClass.hIcon = static_cast<HICON>(LoadImage(GetInstance(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,256,256, NULL));
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = GetName();
	windowClass.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, NULL));
	RegisterClassEx(&windowClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName()
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInstance;
}

bool Window::Init(Application* pApplication, Config* config)
{
	this->width = config->width;
	this->height = config->height;
	DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT windowRect;
	windowRect.left = GetSystemMetrics(SM_CXSCREEN) * 0.5f - this->width * 0.5f;
	windowRect.top = GetSystemMetrics(SM_CYSCREEN) * 0.5f - this->height * 0.5f;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	if (AdjustWindowRect(&windowRect, windowStyle, FALSE) == FALSE)
	{
		ErrorLogger::Log(GetLastError(), "AdjustWindowRect Failed");
		return false;
	}

	this->hWnd = CreateWindow(
		WindowClass::GetName(),config->name.c_str(),
		windowStyle,
		windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr,WindowClass::GetInstance(), pApplication);

	if (this->hWnd == nullptr)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + config->name);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetWindowTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == FALSE)
	{
		ErrorLogger::Log(GetLastError(), "SetWindowTitle Failed. Param: " + title);
	}
}

bool Window::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (PeekMessage(&msg, this->hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->hWnd))
		{
			this->hWnd = nullptr;
			return false;
		}
	}

	return true;
}


const HWND Window::GetHandle()
{
	return this->hWnd;
}

const int Window::GetWidth()
{
	return this->width;
}

const int Window::GetHeight()
{
	return this->height;
}


LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Application* pWindow = reinterpret_cast<Application*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			ErrorLogger::Log("Critical Error: Pointer to Window is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMessageRedirect));
		return pWindow->WindowProc(hWnd, msg, wParam, lParam);
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

LRESULT WINAPI Window::HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) // Redirects to member fucntion
{
	switch (msg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
		return 0;
	}
	default:
	{
		Application* const pWindow = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return pWindow->WindowProc(hWnd, msg, wParam, lParam);
	}
	}
}