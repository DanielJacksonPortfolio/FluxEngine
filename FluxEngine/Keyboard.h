#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
private:
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			PRESS,
			RELEASE,
			INVALID
		};

		Event();
		Event(Type type, const unsigned char key);
		const bool IsPress();
		const bool IsRelease();
		const bool IsValid();
		const unsigned char GetKeyCode();
	private:
		Type type;
		unsigned char keycode;
	};
public:
	bool KeyIsPressed(const unsigned char keycode);
	Event ReadKey(); 
	bool KeyBufferIsEmpty();
	void ClearKey();

	unsigned char ReadChar();
	bool CharBufferIsEmpty();
	void ClearChar();

	void Clear();

	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();

	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();

private:
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);

	void ClearState();

	bool autoRepeatKeys = false;
	bool autoRepeatChars = true;
	//bool keyStates[256];
	std::bitset<256> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<unsigned char> charBuffer;
};