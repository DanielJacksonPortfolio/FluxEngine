#include "Window.h"

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

const char* Window::WindowClass::GetName()
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInstance;
}

Window::Window(Config* config) : width(config->width), height(config->height)
{
	DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	RECT windowRect;
	windowRect.left = GetSystemMetrics(SM_CXSCREEN) * 0.5f - this->width * 0.5f;
	windowRect.top = GetSystemMetrics(SM_CYSCREEN) * 0.5f - this->height * 0.5f;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;
	if (AdjustWindowRect(&windowRect, windowStyle, FALSE) == FALSE)
	{
		ErrorLogger::Log(GetLastError(), "AdjustWindowRect Failed");
	}

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

void Window::SetWindowTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == FALSE)
	{
		ErrorLogger::Log(GetLastError(), "SetWindowTitle Failed. Param: " + title);
	}
}

LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

LRESULT WINAPI Window::HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) // Redirects to member fucntion
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWindow->WindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	//	return true;
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		keyboard.ClearState();
		mouse.ClearState();

	//KEYBOARD MESSAGES
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat() || !(lParam & 0x40000000))
			keyboard.OnKeyPressed(keycode);
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(keycode);
		break;
	}
	case WM_CHAR:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.IsCharsAutoRepeat() || !(lParam & 0x40000000))
		{
			keyboard.OnChar(ch);
		}
		break;
	}

	// Mouse Messages //

	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;

		if (x >= 0 && x < this->width && y >= 0 && y < this->height) //Client in region, log move
		{
			mouse.OnMouseMove(x, y);
			if (!this->mouse.IsInWindow()) //If not previously in region, log enter
			{
				SetCapture(hWnd);
				this->mouse.OnMouseEnter();
			}
		}
		else // Not in region
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) //If button down, maintain capture
			{
				this->mouse.OnMouseMove(x, y);
			}
			else //Leaving window without button pressed, log leave
			{
				ReleaseCapture();
				this->mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnLeftPressed(x, y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnLeftReleased(x, y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnRightPressed(x, y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnRightReleased(x, y);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnMiddlePressed(x, y);
		break;
	}
	case WM_MBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		mouse.OnMiddleReleased(x, y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		int x = pt.x;
		int y = pt.y;
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(x, y);
		}
		break;
	}
	case WM_INPUT:
	{
		UINT dataSize = {0};
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	}//Switch End
	return DefWindowProc(hWnd, msg, wParam, lParam);
}