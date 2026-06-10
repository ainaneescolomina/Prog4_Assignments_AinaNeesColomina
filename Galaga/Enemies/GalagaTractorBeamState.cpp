#include "GalagaTractorBeamState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"

void dae::GalagaTractorBeamState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = glm::vec2{ pos.x, pos.y };

    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(true);
}

void dae::GalagaTractorBeamState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::GalagaTractorBeamState::Update(dae::GameObject* owner, float delta_time)
{
    m_timer += delta_time;

    auto pos = owner->GetTransform().GetPosition();

    switch (m_phase)
    {
    case dae::GalagaTractorBeamState::Phase::Looping:    
    default:
        pos.x += std::sin(m_timer * 2.f) * 8.f * delta_time;
        pos.x += std::sin(m_timer * 2.f) * 8.f * delta_time;
        break;
    case dae::GalagaTractorBeamState::Phase::MoveDown:
        break;
    case dae::GalagaTractorBeamState::Phase::TractorBeam:
        break;
    case dae::GalagaTractorBeamState::Phase::Return:
        break;
    }

    // Small curve
    pos.x += std::sin(m_timer * 2.f) * 8.f * delta_time;
    pos.y += m_speed * delta_time;

    owner->SetPosition(pos.x, pos.y);

    if (pos.y > 850.f)
        return std::make_unique<dae::EnemyFormationState>(200.f, m_startPos);

    return nullptr;
}
