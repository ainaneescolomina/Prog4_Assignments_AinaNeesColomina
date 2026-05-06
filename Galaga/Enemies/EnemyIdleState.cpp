#include "EnemyIdleState.h"
#include "GameObject.h"
#include <random>
#include "BeeEnemyDiveState.h"

void dae::EnemyIdleState::OnEnter(dae::GameObject*)
{
	m_waitAttack = static_cast<float>(rand() % 15 + 5);
}

void dae::EnemyIdleState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyIdleState::Update(dae::GameObject* owner, float delta_time)
{
    m_timer += delta_time;

    auto pos = owner->GetTransform().GetPosition();
    //float xOffset = std::sin(m_timer * 2.f) * 0.5f;
    //owner->SetPosition(pos.x + xOffset, pos.y);

    if (m_timer >= m_waitAttack) 
    {
        return std::make_unique<dae::BeeEnemyDiveState>();
    }

    return nullptr;
}
