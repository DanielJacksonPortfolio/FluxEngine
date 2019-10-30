#include "Application.h"

bool Application::Init(Config* config)
{
	timer.Start();

	if (!this->window.Init(this, config))
		return false;
}

bool Application::ProcessMessages()
{
	return window.ProcessMessages();
}

void Application::Update()
{
	float dt = timer.GetMilisecondsElapsed();

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
		if (mouse.IsRightPressed())
		{
			if (me.GetType() == Mouse::Event::Type::RAW_MOVE)
			{
				//this->gfx.camera.AdjustRotation(static_cast<float>(me.GetPosY()) * 0.01f, static_cast<float>(me.GetPosX()) * 0.01f, 0.0f);
			}
		}
	}


	timer.Restart();
}

void Application::RenderFrame()
{
}
