#include "EnemyFormationState.h"
#include "GameObject.h"
#include "EnemyComponents.h"
#include "EnemyIdleState.h"

void dae::EnemyFormationState::OnEnter(dae::GameObject* owner)
{
    if (!owner) return;
    
    auto* enemyComp = owner->GetComponent<EnemyComponent>();
    auto pos = owner->GetTransform().GetPosition(); 
    m_targetPos = enemyComp? enemyComp->GetFormationPosition() : glm::vec2(pos.x, pos.y);
}

void dae::EnemyFormationState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyFormationState::Update(dae::GameObject* owner, float delta_time)
{
    auto ownerPos = owner->GetTransform().GetPosition();
    glm::vec2 pos{ ownerPos.x, ownerPos.y };
    glm::vec2 direction = m_targetPos - pos;
    float distance = glm::length(direction);

    if (distance < 5.f) {
        owner->SetPosition(m_targetPos.x, m_targetPos.y);
        return std::make_unique<dae::EnemyIdleState>();
    }

    direction = glm::normalize(direction);
    pos += direction * m_speed * delta_time;
    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}
