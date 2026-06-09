#include "EnemyStateComponent.h"

EnemyStateComponent::~EnemyStateComponent() = default;

void EnemyStateComponent::Update(float delta_time)
{
    if (m_pCurrentState) 
    {
        auto nextState = m_pCurrentState->Update(GetOwner(), delta_time);
        if (nextState) 
        {
            SetState(std::move(nextState));
        }
    }
}

void EnemyStateComponent::SetState(std::unique_ptr<dae::EnemyState> newState)
{
    if (m_pCurrentState) m_pCurrentState->OnExit(GetOwner());
    m_pCurrentState = std::move(newState);
    if (m_pCurrentState) m_pCurrentState->OnEnter(GetOwner());
}
