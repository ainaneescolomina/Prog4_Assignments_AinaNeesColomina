#include "InputManager.h"
#include "InputManager.h"
#include "InputManager.h"
#include <backends/imgui_impl_sdl3.h>
#include <algorithm>

dae::InputManager::InputManager()
{
	for (int i = 0; i < 4; ++i)
	{
		m_pGamepads.push_back(std::make_unique<dae::Gamepad>(i));
	}
}

bool dae::InputManager::ProcessInput(float delta_time)
{
	for (auto& gamepad : m_pGamepads)
	{
		gamepad->Update();
	}

	// Controller input
	for (auto& binding : m_pGamepadBindings)
	{
		unsigned int idx = binding->controllerIdx;
		if (idx >= m_pGamepads.size()) continue;

		bool trigger = false;
		switch (binding->state)
		{
		case KeyState::Down:   
			trigger = m_pGamepads[idx]->IsDown(binding->button); 
			break;

		case KeyState::Pressed: 
			trigger = m_pGamepads[idx]->IsPressed(binding->button); 
			break;

		case KeyState::Up:      
			trigger = m_pGamepads[idx]->IsUp(binding->button); 
			break;
		}

		if (trigger) 
			binding->command->Execute(delta_time);
	}

	for (auto& binding : m_pAxisBindings)
	{
		unsigned int idx = binding->controllerIdx;
		if (idx >= m_pGamepads.size()) continue;

		float stickX = m_pGamepads[idx]->GetLeftStickX();
		if (std::abs(stickX) > 0.1f)
		{
			// Pass the live stickX value (-1.0f to 1.0f)
			binding->command->Execute(delta_time, stickX);
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

	for (auto& binding : m_pBindings)
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
			binding->command->Execute(delta_time);
	}

	return true;
}

void dae::InputManager::BindCommand(SDL_Keycode key, KeyState state, std::unique_ptr<Command> command)
{
	auto newInputBinding = std::make_unique<InputBinding>(key, state, std::move(command));
	m_pBindings.push_back(std::move(newInputBinding));
}

void dae::InputManager::BindGamepadCommand(unsigned int controllerIndex, unsigned int button, KeyState state, std::unique_ptr<Command> command)
{
	auto newGamepadBinding = std::make_unique<GamepadBinding>(button, state, controllerIndex, std::move(command));
	m_pGamepadBindings.push_back(std::move(newGamepadBinding));
}

void dae::InputManager::BindAxisCommand(unsigned int controllerIndex, std::unique_ptr<Command> command)
{
	auto newAxisBinding = std::make_unique<AxisBinding>(controllerIndex, std::move(command));
	m_pAxisBindings.push_back(std::move(newAxisBinding));
}

void dae::InputManager::UnbindCommand(SDL_Keycode key, KeyState state)
{
	m_pBindings.erase(
		std::remove_if(
			m_pBindings.begin(),
			m_pBindings.end(),
			[key, state](const std::unique_ptr<InputBinding>& binding)
			{
				return binding->key == key && binding->state == state;
			}
		),
		m_pBindings.end()
	);
}

void dae::InputManager::UnbindGamepadCommand(unsigned int button, KeyState state)
{
	m_pGamepadBindings.erase(
		std::remove_if(
			m_pGamepadBindings.begin(),
			m_pGamepadBindings.end(),
			[button, state](const std::unique_ptr<GamepadBinding>& binding)
			{
				return binding->button == button && binding->state == state;
			}
		),
		m_pGamepadBindings.end()
	);
}

void dae::InputManager::ClearAllBindings()
{
	m_pBindings.clear();
	m_pGamepadBindings.clear();
	m_pAxisBindings.clear();
}


