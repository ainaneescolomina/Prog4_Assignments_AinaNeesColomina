#include "BeeEnemyDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"

void dae::BeeEnemyDiveState::OnEnter(dae::GameObject* owner)
{
	//owner->GetComponent<dae::RenderComponent>()->SetTexture("enemy_butterfly_dive.png");
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = glm::vec2{ pos.x, pos.y };
}

void dae::BeeEnemyDiveState::OnExit(dae::GameObject*){}

std::unique_ptr<dae::EnemyState> dae::BeeEnemyDiveState::Update(dae::GameObject* owner, float delta_time)
{
    auto pos = owner->GetTransform().GetPosition();
    pos.y += m_speed * delta_time;
    owner->SetPosition(pos.x, pos.y);

    if (pos.y > 750) 
    {
        //owner->SetPosition(pos.x, -50);
        return std::make_unique<dae::EnemyFormationState>(150.f, m_startPos);
    }

    return nullptr;
}
