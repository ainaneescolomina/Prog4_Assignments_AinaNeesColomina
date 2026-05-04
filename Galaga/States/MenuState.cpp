#include "MenuState.h"
#include "PlayState.h"
#include "InputManager.h"

std::unique_ptr<dae::GameState> dae::MenuState::Update(float)
{
    return nullptr;
}

void dae::MenuState::OnEnter()
{
    auto pCommand = std::make_unique<StartGameCommand>(m_pManager);
    InputManager::GetInstance().BindCommand(SDLK_SPACE, KeyState::Down, std::move(pCommand));
}

void dae::MenuState::OnExit()
{
    InputManager::GetInstance().UnbindGamepadCommand(SDLK_SPACE, KeyState::Down);
}

void dae::StartGameCommand::Execute(float)
{
    m_pManager->ChangeState(std::make_unique<PlayState>());
}
