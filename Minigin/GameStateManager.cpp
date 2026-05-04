#include "GameStateManager.h"

void dae::GameStateManager::Update(float deltaTime)
{
    if (m_pCurrentState) 
    {
        auto pNewState = m_pCurrentState->Update(deltaTime);

        if (pNewState != nullptr) {
            ChangeState(std::move(pNewState));
        }
    }
}

void dae::GameStateManager::ChangeState(std::unique_ptr<dae::GameState> newState)
{
    if (m_pCurrentState) m_pCurrentState->OnExit();
    m_pCurrentState = std::move(newState);
    m_pCurrentState->OnEnter();
}
