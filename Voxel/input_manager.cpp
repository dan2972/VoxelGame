#include "input_manager.h"

std::map<InputManager::KeyboardControl, SDL_Scancode> InputManager::m_keyControlMap;
const Uint8* InputManager::m_keyState;

void InputManager::update() {
	m_keyState = SDL_GetKeyboardState(nullptr);
}

void InputManager::mapControl(KeyboardControl control, SDL_Scancode code) {
	m_keyControlMap.insert({ control, code });
}

bool InputManager::isKeyDown(KeyboardControl control) {
	if (m_keyControlMap.find(control) == m_keyControlMap.end())
		throw KeyMappingDoesNotExistException("The control does not exist");
	return m_keyState[m_keyControlMap.at(control)];
}