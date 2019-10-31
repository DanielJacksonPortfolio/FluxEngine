#pragma once
#include "../Tools/StrippedWindows.h"
#include<queue>
struct MousePoint
{
	int x, y;
};

class Mouse
{
private:
	friend class Application;
public:
	class Event
	{
	public:
		enum class Type
		{
			LEFT_PRESS,
			LEFT_RELEASE,
			RIGHT_PRESS,
			RIGHT_RELEASE,
			MIDDLE_PRESS,
			MIDDLE_RELEASE,
			WHEEL_UP,
			WHEEL_DOWN,
			MOVE,
			ENTER,
			LEAVE,
			RAW_MOVE,
			INVALID
		};

		Event();
		Event(Type type, const int x, const int y);
		const bool IsValid();
		const Type GetType();
		const MousePoint GetPos();
		const int GetPosX();
		const int GetPosY();
	private:
		Type type;
		int x, y;
	};
public:
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsMiddlePressed();
	bool IsInWindow();

	int GetPosX();
	int GetPosY();
	MousePoint GetPos();

	bool EventBufferIsEmpty();
	Event ReadEvent();
private:
	void ClearState();

	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnWheelDelta(int x, int y, int delta);
	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnMouseMoveRaw(int x, int y);

	std::queue<Event> eventBuffer;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool middleIsPressed = false;
	bool isInWindow = false;
	int wheelDeltaCarry = 0;
	int x = 0, y = 0;
};

