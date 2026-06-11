#include "EnemyComponents.h"
#include "GameObject.h"
#include "GameComponents.h"
#include "EnemyDeadState.h"

dae::EnemyComponent::~EnemyComponent()
{
    if (m_pBeamObject)
        m_pBeamObject->MarkForDestroy();
}

void dae::EnemyComponent::Notify(Event event, void*)
{
    if (event.id == make_sdbm_hash("TakeDamage"))
    {
        if (m_type == EnemyType::BossGalaga )
        {
            auto* myRenderComp = GetOwner()->GetComponent<dae::AnimatedRenderComponent>();
            if (myRenderComp)
                myRenderComp->SetTexture("Images/enemy_galaga_hurt.png", true);
        }
    }
    else if (event.id == make_sdbm_hash("ActorDied"))
    {
        auto* myStateComp = GetOwner()->GetComponent<EnemyStateComponent>();
        if (myStateComp)
            myStateComp->SetState(std::make_unique<dae::EnemyDeadState>());
    }
}

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
