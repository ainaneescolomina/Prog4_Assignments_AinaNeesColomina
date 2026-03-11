#pragma once
#include "Singleton.h"
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "Commands.h"
#include "Gamepad.h"

namespace dae
{
	enum class KeyState
	{
		Down,
		Up,
		Pressed
	};

	struct InputBinding
	{
		SDL_Keycode key;
		KeyState state;
		std::unique_ptr<Command> command;
	};

	struct GamepadBinding
	{
		unsigned int button;
		KeyState state;
		std::unique_ptr<Command> command;
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();

		bool ProcessInput();

		void BindCommand(SDL_Keycode key, KeyState state, std::unique_ptr<Command> command);
		void BindGamepadCommand(unsigned int button, KeyState state, std::unique_ptr<Command> command);

		void UnbindCommand(SDL_Keycode key, KeyState state);

	private:
		std::unique_ptr<dae::Gamepad> m_gamepad;

		std::vector<std::unique_ptr<InputBinding>> m_bindings;
		std::vector<std::unique_ptr<GamepadBinding>> m_gamepadBindings;
	};

}
