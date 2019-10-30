#pragma once
#include "WindowContainer.h"
#include "Timer.h"

class Application : public WindowContainer
{
public:
	//Application(Config* config);
	bool Init(Config* config);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

private:
	Timer timer;
};

