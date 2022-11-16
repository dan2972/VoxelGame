#pragma once
#include <map>
#include <SDL/SDL.h>
#include <stdexcept>

class KeyMappingDoesNotExistException : public std::runtime_error {
public:
	KeyMappingDoesNotExistException(const std::string& msg) : runtime_error(msg.c_str()) {}
};

class InputManager
{
public:
	enum KeyboardControl {
		MoveLeft,
		MoveRight,
		MoveForward,
		MoveBackward,
		MoveUp,
		MoveDown
	};

	static void update();
	static void mapControl(KeyboardControl control, SDL_Scancode code);
	static bool isKeyDown(KeyboardControl control);

private:
	static std::map<KeyboardControl, SDL_Scancode> m_keyControlMap;
	static const Uint8* m_keyState;

};

