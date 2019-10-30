#pragma once
#include "ErrorLogger.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Config.h"
#include "resource.h"

class Window
{
private:
	class WindowClass
	{
	public:
		static const char* GetName();
		static HINSTANCE GetInstance();
	private:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "FluxEngine Window";
		static WindowClass wndClass;
		HINSTANCE hInstance;
	};

public:
	Window(Config* config);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	int Update();
	void SetWindowTitle(const std::string& title);

	Keyboard keyboard;
	Mouse mouse;

private:
	int width;
	int height;
	HWND hWnd;

	static LRESULT WINAPI HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

