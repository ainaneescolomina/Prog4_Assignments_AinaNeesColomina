#include "EnemyIdleState.h"
#include "GameObject.h"
#include "EnemyComponents.h"

void dae::EnemyIdleState::OnEnter(dae::GameObject* owner)
{
    if (owner)
    {
        auto* enemyComp = owner->GetComponent<EnemyComponent>();
        auto pos = owner->GetTransform().GetPosition();
        m_formationPosition = enemyComp ? enemyComp->GetFormationPosition() : glm::vec2(pos.x, pos.y);
    }
}

void dae::EnemyIdleState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyIdleState::Update(dae::GameObject* owner, float)
{
    owner->SetPosition(m_formationPosition.x + m_formationOffset, m_formationPosition.y);
    return nullptr;
}
