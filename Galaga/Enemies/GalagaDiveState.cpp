#include "GalagaDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"

void dae::GalagaDiveState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = glm::vec2{ pos.x, pos.y };

    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(true);
}

void dae::GalagaDiveState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::GalagaDiveState::Update(dae::GameObject* owner, float delta_time)
{
    m_timer += delta_time;

    auto pos = owner->GetTransform().GetPosition();

    // Small curve
    pos.x += std::sin(m_timer * 2.f) * 80.f * delta_time;
    pos.y += m_speed * delta_time;

    owner->SetPosition(pos.x, pos.y);

    if (pos.y > 850.f)
        return std::make_unique<dae::EnemyFormationState>(200.f, m_startPos);

    return nullptr;
}
