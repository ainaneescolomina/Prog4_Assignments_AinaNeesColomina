#include "EnemyComponents.h"

dae::EnemyComponent::~EnemyComponent() = default;
EnemyStateComponent::~EnemyStateComponent() = default;

int dae::EnemyComponent::GetScoreValue() const
{
    switch (m_type)
    {
    case EnemyType::Bee:
        return m_isDiving? 100 : 50;

    case EnemyType::Butterfly:
        return m_isDiving? 160 : 80;

    case EnemyType::BossGalaga:
        return m_isDiving? 400 : 150;
    }

    return 0;
}

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
