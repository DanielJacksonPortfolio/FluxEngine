#include "Keyboard.h"

/// KEYBOARD EVENT ///

Keyboard::Event::Event() : type(Type::INVALID), keycode(0u)
{
}

Keyboard::Event::Event(Type type, const unsigned char keycode) : type(type), keycode(keycode)
{
}

const bool Keyboard::Event::IsPress()
{
	return this->type == Type::PRESS;
}

const bool Keyboard::Event::IsRelease()
{
	return this->type == Type::RELEASE;
}

const bool Keyboard::Event::IsValid()
{
	return this->type != Type::INVALID;
}

const unsigned char Keyboard::Event::GetKeyCode()
{
	return this->keycode;
}

/// KEYBOARD ///

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey()
{
	if (this->KeyBufferIsEmpty())
		return Keyboard::Event();
	else
	{
		Keyboard::Event e = this->keyBuffer.front();
		this->keyBuffer.pop();
		return e;
	}
}

bool Keyboard::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

void Keyboard::ClearKey()
{
	this->keyBuffer = std::queue<Event>();
}

unsigned char Keyboard::ReadChar()
{
	if (this->CharBufferIsEmpty())
		return 0u;
	else
	{
		unsigned char charcode = this->charBuffer.front();
		this->charBuffer.pop();
		return charcode;
	}
}

bool Keyboard::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

void Keyboard::ClearChar()
{
	this->charBuffer = std::queue<unsigned char>();
}

void Keyboard::Clear()
{
	ClearKey();
	ClearChar();
}

void Keyboard::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars()
{
	this->autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat()
{
	return this->autoRepeatChars;
}

void Keyboard::OnKeyPressed(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.push(Event(Event::Type::PRESS, key));
}

void Keyboard::OnKeyReleased(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(Event(Event::Type::RELEASE, key));
}

void Keyboard::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

void Keyboard::ClearState()
{
	keyStates.reset();
}
