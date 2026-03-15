#pragma once
#include "Singleton.h"
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "Commands.h"
#include "Gamepad.h"
#include <unordered_map>

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

		bool ProcessInput(float delta_time);

		void BindCommand(SDL_Keycode key, KeyState state, std::unique_ptr<Command> command);
		void BindGamepadCommand(unsigned int button, KeyState state, std::unique_ptr<Command> command);

		void UnbindCommand(SDL_Keycode key, KeyState state);
		void UnbindGamepadCommand(unsigned int button, KeyState state);

	private:
		std::unique_ptr<dae::Gamepad> m_pGamepad;

		std::vector<std::unique_ptr<InputBinding>> m_pBindings;
		std::vector<std::unique_ptr<GamepadBinding>> m_pGamepadBindings;

		std::unordered_map<SDL_Keycode, bool> m_keysDown;
	};

}
