#include "EnemyDeadState.h"
#include "GameObject.h"
#include "EnemyComponents.h"
#include "ServiceLocator.h"

void dae::EnemyDeadState::OnEnter(GameObject* owner)
{
    if (!owner) return;

    if (auto* collider = owner->GetComponent<ColliderComponent>())
        collider->SetActive(false);

    if (auto* renderComp = owner->GetComponent<AnimatedRenderComponent>())
        renderComp->SetTexture("Images/enemy_explosion.png", 1, 5, 0.2f, true);

    auto& sound = dae::servicelocator::get_sound_system();
    sound_id soundId = 3;

    if (auto* enemyComp = owner->GetComponent<dae::EnemyComponent>(); enemyComp
        && enemyComp->GetType() == EnemyType::BossGalaga)
        soundId = 4;

    sound.Play(soundId, 0.5f);
}

std::unique_ptr<dae::EnemyState> dae::EnemyDeadState::Update(GameObject* owner, float delta_time)
{
    m_deathTimer -= delta_time;

    if (m_deathTimer <= 0.f)
        owner->MarkForDestroy();

    return nullptr;
}