#include "Window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEXA windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = HandleMessageSetup;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetInstance();
	windowClass.hIcon = static_cast<HICON>(LoadImage(GetInstance(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,256,256,NULL));
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

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInstance;
}

Window::Window(Config* config) noexcept
{
	this->width = config->width;
	this->height = config->height;

	DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT windowRect;
	windowRect.left = GetSystemMetrics(SM_CXSCREEN) * 0.5f - this->width * 0.5f;
	windowRect.top = GetSystemMetrics(SM_CYSCREEN) * 0.5f - this->height * 0.5f;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	this->hWnd = CreateWindow(
		WindowClass::GetName(),config->name.c_str(),
		windowStyle,
		windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr,WindowClass::GetInstance(), this);

	if (this->hWnd == nullptr)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + config->name);
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

int Window::Update()
{
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
}

LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
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

LRESULT WINAPI Window::HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept // Redirects to member fucntion
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWindow->WindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	//	return true;
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_CHAR: //Text Input / Capital Differentiation
		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}