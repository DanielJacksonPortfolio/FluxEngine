#pragma once
#include "ErrorLogger.h"
#include "Config.h"
#include "resource.h"

class WindowContainer;

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
		static constexpr const char* wndClassName = "FluxEngine Window";
		static WindowClass wndClass;
		HINSTANCE hInstance;
	};

public:
	bool Init(WindowContainer* pWindowContainer, Config* config);
	~Window();
	void SetWindowTitle(const std::string& title);
	bool ProcessMessages();
	const HWND GetHandle();

private:
	int width;
	int height;
	HWND hWnd;

	static LRESULT WINAPI HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

