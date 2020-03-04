#include "Mouse.h"

Mouse::Event::Event() : type(Type::INVALID), x(0), y(0)
{
}

Mouse::Event::Event(Type type, const int x, const int y) : type(type), x(x), y(y)
{
}

const bool Mouse::Event::IsValid()
{
	return this->type != Type::INVALID;
}

const Mouse::Event::Type Mouse::Event::GetType()
{
	return this->type;
}

const MousePoint Mouse::Event::GetPos()
{
	return { this->x, this->y };
}

const int Mouse::Event::GetPosX()
{
	return this->x;
}

const int Mouse::Event::GetPosY()
{
	return  this->y;
}


float Mouse::GetMouseDiffX()
{
	return this->xDiff;
}

float Mouse::GetMouseDiffY()
{
	return this->yDiff;
}

void Mouse::AddMouseDiffX(float diffX)
{
	this->xDiff += diffX;
}

void Mouse::AddMouseDiffY(float diffY)
{
	this->yDiff += diffY;
}

void Mouse::ResetMouseDiff()
{
	this->xDiff = 0.0f;
	this->yDiff = 0.0f;
}

void Mouse::OnLeftPressed(int x, int y)
{
	this->leftIsPressed = true;
	this->eventBuffer.push(Event(Event::Type::LEFT_PRESS, x, y));
}

void Mouse::OnLeftReleased(int x, int y)
{
	this->leftIsPressed = false;
	this->eventBuffer.push(Event(Event::Type::LEFT_RELEASE, x, y));
}

void Mouse::OnRightPressed(int x, int y)
{
	this->rightIsPressed = true;
	this->eventBuffer.push(Event(Event::Type::RIGHT_PRESS, x, y));
}

void Mouse::OnRightReleased(int x, int y)
{
	this->rightIsPressed = false;
	this->eventBuffer.push(Event(Event::Type::RIGHT_RELEASE, x, y));
}

void Mouse::OnMiddlePressed(int x, int y)
{
	this->middleIsPressed = true;
	this->eventBuffer.push(Event(Event::Type::MIDDLE_PRESS, x, y));
}

void Mouse::OnMiddleReleased(int x, int y)
{
	this->middleIsPressed = false;
	this->eventBuffer.push(Event(Event::Type::MIDDLE_RELEASE, x, y));
}

void Mouse::OnWheelUp(int x, int y)
{
	this->eventBuffer.push(Event(Event::Type::WHEEL_UP, x, y));
}

void Mouse::OnWheelDown(int x, int y)
{
	this->eventBuffer.push(Event(Event::Type::WHEEL_DOWN, x, y));
}

void Mouse::OnWheelDelta(int x, int y, int delta)
{
	wheelDeltaCarry += delta;

	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::OnMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;
	this->eventBuffer.push(Event(Event::Type::MOVE, this->x, this->y));
}

void Mouse::OnMouseLeave()
{
	this->isInWindow = false;
	this->eventBuffer.push(Event(Event::Type::LEAVE, this->x, this->y ));
}

void Mouse::OnMouseEnter()
{
	this->isInWindow = true;
	this->eventBuffer.push(Event(Event::Type::ENTER, this->x, this->y));
}

void Mouse::OnMouseMoveRaw(int x, int y)
{
	this->eventBuffer.push(Event(Event::Type::RAW_MOVE, x, y));
}

bool Mouse::IsLeftPressed()
{
	return this->leftIsPressed;
}

bool Mouse::IsRightPressed()
{
	return this->rightIsPressed;
}

bool Mouse::IsMiddlePressed()
{
	return this->middleIsPressed;
}

bool Mouse::IsInWindow()
{
	return isInWindow;
}

int Mouse::GetPosX()
{
	return this->x;
}

int Mouse::GetPosY()
{
	return this->y;
}

MousePoint Mouse::GetPos()
{
	return { this->x,this->y };
}

bool Mouse::EventBufferIsEmpty()
{
	return this->eventBuffer.empty();
}

Mouse::Event Mouse::ReadEvent()
{
	if (this->eventBuffer.empty())
	{
		return Event();
	}
	else
	{
		Event e = this->eventBuffer.front();
		eventBuffer.pop();
		return e;
	}

}

void Mouse::ClearState()
{
	this->leftIsPressed = false;
	this->rightIsPressed = false;
	this->middleIsPressed = false;
}
