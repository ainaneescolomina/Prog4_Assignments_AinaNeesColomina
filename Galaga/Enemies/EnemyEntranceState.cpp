#include "EnemyEntranceState.h"
#include "GameObject.h"
#include "EnemyIdleState.h"

void dae::EnemyEntranceState::OnEnter(dae::GameObject*){}
void dae::EnemyEntranceState::OnExit(dae::GameObject*) {}

std::unique_ptr<dae::EnemyState> dae::EnemyEntranceState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_currentPoint >= static_cast<int>(m_entrancePath.size()))
        return std::make_unique<dae::EnemyIdleState>();

    auto ownerPos = owner->GetTransform().GetPosition();
    glm::vec2 pos{ ownerPos.x, ownerPos.y };
    glm::vec2 target = m_entrancePath[m_currentPoint];
    glm::vec2 dir = target - pos;

    float dist = glm::length(dir);

    // reached current waypoint
    if (dist < 12.f)
    {
        ++m_currentPoint;
        return nullptr;
    }

    dir = glm::normalize(dir);
    pos += dir * m_speed * delta_time;
    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}
