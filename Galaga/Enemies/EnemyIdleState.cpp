#include "EnemyIdleState.h"
#include "GameObject.h"

void dae::EnemyIdleState::OnEnter(dae::GameObject* owner)
{
    if (owner)
    {
        auto pos = owner->GetTransform().GetPosition();
        m_formationPosition = glm::vec2(pos.x, pos.y);
    }
}

void dae::EnemyIdleState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyIdleState::Update(dae::GameObject* owner, float)
{
    owner->SetPosition(m_formationPosition.x + m_formationOffset, m_formationPosition.y);
    return nullptr;
}
