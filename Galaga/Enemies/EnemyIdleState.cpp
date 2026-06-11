#include "EnemyIdleState.h"
#include "GameObject.h"

#include <glm/glm.hpp>

void dae::EnemyIdleState::OnEnter(dae::GameObject*){}

void dae::EnemyIdleState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyIdleState::Update(dae::GameObject* owner, float)
{
    auto ownerPos = owner->GetTransform().GetPosition();
    owner->SetPosition(ownerPos.x + m_formationOffset, ownerPos.y);

    return nullptr;
}
