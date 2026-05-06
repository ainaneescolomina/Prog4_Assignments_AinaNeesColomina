#include "GameStateManager.h"

void dae::GameStateManager::Update(float delta_time)
{
    if (m_pCurrentState) 
    {
        auto pNewState = m_pCurrentState->Update(delta_time);

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