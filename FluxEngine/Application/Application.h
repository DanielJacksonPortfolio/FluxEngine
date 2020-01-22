#pragma once
#include "../Windows/Window.h"
#include "../Windows/Keyboard.h"
#include "../Engine/DirectX/GraphicsHandler.h"
#include "../Windows/Mouse.h"
#include "../Tools/Timer.h"

class Application
{
public:
	bool Init(Config* config);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

	Window window;
	Keyboard keyboard;
	Mouse mouse;
	GraphicsHandler gfx;

	Application();
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	Timer timer;
	bool qPressed = false;
};

