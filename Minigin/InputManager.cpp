#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include <algorithm>


dae::InputManager::InputManager()
{
	m_gamepad = std::make_unique<dae::Gamepad>(0);
	//BindCommand();
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
	while (SDL_PollEvent(&e)) {
		//if (e.type == SDL_EVENT_QUIT) {
		//	return false;
		//}
		//if (e.type == SDL_EVENT_KEY_DOWN) {
		//}
		//if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		//}
		// etc...

		if (e.type == SDL_EVENT_QUIT)
			return false;

		KeyState eventState;

		if (e.type == SDL_EVENT_KEY_DOWN)
			eventState = KeyState::Down;
		else if (e.type == SDL_EVENT_KEY_UP)
			eventState = KeyState::Up;
		else
		{
			ImGui_ImplSDL3_ProcessEvent(&e);
			continue;
		}
	
		for (auto& binding : m_bindings)
		{
			if (binding->key == e.key.key && binding->state == eventState)
			{
				binding->command->Execute();
			}
		}

		//process event for IMGUI
		ImGui_ImplSDL3_ProcessEvent(&e);
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


