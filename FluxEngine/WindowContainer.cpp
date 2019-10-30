#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer()
{
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			ErrorLogger::Log(GetLastError(), "RegisterRawInputDevices Failed.");
			exit(-1);
		}
		raw_input_initialized = true;
	}

}

LRESULT WindowContainer::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	//	return true;
	switch (msg)
	{
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
		//const POINTS pt = MAKEPOINTS(lParam);
		//int x = pt.x;
		//int y = pt.y;

		//if (x >= 0 && x < this->width && y >= 0 && y < this->height) //Client in region, log move
		//{
		//	mouse.OnMouseMove(x, y);
		//	if (!this->mouse.IsInWindow()) //If not previously in region, log enter
		//	{
		//		SetCapture(hWnd);
		//		this->mouse.OnMouseEnter();
		//	}
		//}
		//else // Not in region
		//{
		//	if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) //If button down, maintain capture
		//	{
		//		this->mouse.OnMouseMove(x, y);
		//	}
		//	else //Leaving window without button pressed, log leave
		//	{
		//		ReleaseCapture();
		//		this->mouse.OnMouseLeave();
		//	}
		//}
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
		mouse.OnWheelDelta(x, y, GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
	case WM_INPUT:
	{
		UINT dataSize = { 0 };
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