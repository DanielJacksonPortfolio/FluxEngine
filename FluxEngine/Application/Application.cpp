#include "Application.h"
#include <memory>

Application::Application()
{
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			ErrorLogger::Log(GetLastError(), "RegisterRawInputDevices Failed.");
			exit(-1);
		}
		raw_input_initialized = true;
	}

}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Application::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_KILLFOCUS:
		keyboard.ClearState();
		mouse.ClearState();

		/// KEYBOARD MESSAGES ///
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

	/// MOUSE MESSAGES ///

	case WM_MOUSEMOVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_RBUTTONDOWN:
	case WM_RBUTTONUP: case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		switch (msg)
		{
		case WM_MOUSEMOVE:
		{
			if (pt.x >= 0 && pt.x < this->window.GetWidth() && pt.y >= 0 && pt.y < this->window.GetHeight()) //Client in region, log move
			{
				mouse.OnMouseMove(pt.x, pt.y);
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
					this->mouse.OnMouseMove(pt.x, pt.y);
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
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			mouse.OnMiddlePressed(pt.x, pt.y);
			break;
		}
		case WM_MBUTTONUP:
		{
			mouse.OnMiddleReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			mouse.OnWheelDelta(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		}
		}
	}
	case WM_INPUT:
	{
		UINT dataSize = { 0 };
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));
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

bool Application::Init(Config* config)
{
	timer.Start();

	if (!this->window.Init(this, config))
		return false;

	if (!this->gfx.Init(window.GetHandle(), window.GetWidth(), window.GetHeight(),config))
		return false;

	return true;
}

bool Application::ProcessMessages()
{
	return window.ProcessMessages();
}

void Application::Update()
{
	float dt = static_cast<float>(timer.GetMilisecondsElapsed());

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while (!keyboard.KeyBufferIsEmpty())
	{
		Keyboard::Event kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while (!mouse.EventBufferIsEmpty())
	{
		Mouse::Event me = mouse.ReadEvent();
		if (mouse.IsLeftPressed())
		{
			if (this->gfx.camera != nullptr)
			{
 				RenderableGameObject* object = gfx.SelectObject(mouse.GetPosX(), mouse.GetPosY());
				if (object != nullptr)
				{
					gfx.currentObject = object;
				}
			}
		}
		if (mouse.IsRightPressed())
		{
			if (me.GetType() == Mouse::Event::Type::RAW_MOVE)
			{
				if (this->gfx.camera != nullptr)
				{
					if (!this->gfx.camera->GetLookAtMode())
					{
						if (this->gfx.camera->GetUpVectorFloat().y > 0)
							this->gfx.camera->AdjustRotation(static_cast<float>(me.GetPosY()) * 0.01f, static_cast<float>(me.GetPosX()) * 0.01f, 0.0f);
						else
							this->gfx.camera->AdjustRotation(static_cast<float>(me.GetPosY()) * 0.01f, static_cast<float>(me.GetPosX()) * -0.01f, 0.0f);
					}
				}
			}
		}
	}

	if (this->gfx.camera != nullptr)
	{
		if (keyboard.KeyIsPressed('W'))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetForwardVector() * gfx.camera->GetVelocity().x * dt);
		}
		if (keyboard.KeyIsPressed('A'))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetLeftVector() * gfx.camera->GetVelocity().x * dt);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetBackwardVector() * gfx.camera->GetVelocity().x * dt);
		}
		if (keyboard.KeyIsPressed('D'))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetRightVector() * gfx.camera->GetVelocity().x * dt);
		}
		if (keyboard.KeyIsPressed(VK_SPACE))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetUpVector() * gfx.camera->GetVelocity().x * dt);
		}
		if (keyboard.KeyIsPressed(VK_SHIFT))
		{
			this->gfx.camera->AdjustPosition(this->gfx.camera->GetDownVector() * gfx.camera->GetVelocity().x * dt);
		}
	}
	timer.Restart();
}

void Application::RenderFrame()
{
	gfx.RenderFrame();
}
