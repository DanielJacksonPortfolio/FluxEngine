#pragma once
#include "Window.h"
#include "Keyboard.h"
#include "Mouse.h"
//#include "Graphics/Graphics.h"

class WindowContainer
{
private:

protected:
	Window window;
	Keyboard keyboard;
	Mouse mouse;
	//Graphics gfx;
public:
	WindowContainer();
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

