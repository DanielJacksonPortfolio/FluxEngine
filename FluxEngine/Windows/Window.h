#pragma once
#include "../Config.h"
#include "../resource.h"

class Application;

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
	bool Init(Application* pApplication, Config* config);
	~Window();
	void SetWindowTitle(const std::string& title);
	bool ProcessMessages();
	const HWND GetHandle();
	const int GetWidth();
	const int GetHeight();

private:
	int width;
	int height;
	HWND hWnd;

	static LRESULT WINAPI HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessageRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

