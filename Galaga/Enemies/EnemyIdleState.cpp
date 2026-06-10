#include "EnemyIdleState.h"
#include "GameObject.h"
#include <random>

#include "EnemyComponents.h"
#include "BeeDiveState.h"
#include "ButterflyDiveState.h"
#include "GalagaDiveState.h"
#include "GalagaTractorBeamState.h"

void dae::EnemyIdleState::OnEnter(dae::GameObject*)
{
	m_waitAttack = static_cast<float>(rand() % 15 + 1);
}

void dae::EnemyIdleState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::EnemyIdleState::Update(dae::GameObject* owner, float delta_time)
{
    m_timer += delta_time;

    auto* enemyComp = owner->GetComponent<EnemyComponent>();

    if (m_timer >= m_waitAttack)
    {
        switch (enemyComp->GetType())
        {
        case EnemyType::Bee:
            return std::make_unique<BeeDiveState>();

        case EnemyType::Butterfly:
            return std::make_unique<ButterflyDiveState>();

        case EnemyType::BossGalaga:
            return std::make_unique<BeeDiveState>(); // placeholder
        }
    }

    return nullptr;
}
