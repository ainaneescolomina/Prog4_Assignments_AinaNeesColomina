#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include <algorithm>

dae::InputManager::InputManager()
{
	m_gamepad = std::make_unique<dae::Gamepad>(0);
}

bool dae::InputManager::ProcessInput()
{
	m_gamepad->Update();

	// Controller input
	for (auto& binding : m_gamepadBindings)
	{
		bool trigger = false;

		switch (binding->state)
		{
		case KeyState::Down:
			trigger = m_gamepad->IsDown(binding->button);
			break;

		case KeyState::Pressed:
			trigger = m_gamepad->IsPressed(binding->button);
			break;

		case KeyState::Up:
			trigger = m_gamepad->IsUp(binding->button);
			break;
		}

		if (trigger)
		{
			binding->command->Execute();
		}
	}

	// Keyboard input
	SDL_Event e;
	std::unordered_map<SDL_Keycode, bool> keysJustPressed;
	std::unordered_map<SDL_Keycode, bool> keysJustReleased;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
			return false;

		if (e.type == SDL_EVENT_KEY_DOWN)
		{
			if (!m_keysDown[e.key.key])
				keysJustPressed[e.key.key] = true;
			m_keysDown[e.key.key] = true;
		}
		else if (e.type == SDL_EVENT_KEY_UP)
		{
			keysJustReleased[e.key.key] = true;
			m_keysDown[e.key.key] = false;
		}

		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	for (auto& binding : m_bindings)
	{
		bool trigger = false;

		switch (binding->state)
		{
		case KeyState::Down:
			trigger = keysJustPressed[binding->key];
			break;
		case KeyState::Pressed:
			trigger = m_keysDown[binding->key];
			break;
		case KeyState::Up:
			trigger = keysJustReleased[binding->key];
			break;
		}

		if (trigger)
			binding->command->Execute();
	}

	return true;
}

void dae::InputManager::BindCommand(SDL_Keycode key, KeyState state, std::unique_ptr<Command> command)
{
	auto newInputBinding = std::make_unique<InputBinding>(key, state, std::move(command));
	m_bindings.push_back(std::move(newInputBinding));
}

void dae::InputManager::BindGamepadCommand(unsigned int button, KeyState state, std::unique_ptr<Command> command)
{
	auto newGamepadBinding = std::make_unique<GamepadBinding>(button, state, std::move(command));
	m_gamepadBindings.push_back(std::move(newGamepadBinding));
}

void dae::InputManager::UnbindCommand(SDL_Keycode key, KeyState state)
{
	m_bindings.erase(
		std::remove_if(
			m_bindings.begin(),
			m_bindings.end(),
			[key, state](const std::unique_ptr<InputBinding>& binding)
			{
				return binding->key == key && binding->state == state;
			}
		),
		m_bindings.end()
	);
}


