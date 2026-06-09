#include "BulletSpawner.h"
#include "GameComponents.h"
#include "GameObject.h"
#include "Scene.h"
#include "ServiceLocator.h"

dae::BulletSpawner::BulletSpawner(dae::Scene& scene)
    : m_scene(scene) 
{
    // PLAYER BULLETS
    for (int i{}; i < maxPlayerBullets; ++i)
    {
        auto bullet = CreateBullet(true);
        m_playerBulletPool.push_back(bullet.get());
        m_scene.Add(std::move(bullet));
    }

    // ENEMY BULLETS
    for (int i{}; i < maxEnemyBullets; ++i)
    {
        auto bullet = CreateBullet(false);
        m_enemyBulletPool.push_back(bullet.get());
        m_scene.Add(std::move(bullet));
    }
}

void dae::BulletSpawner::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("SpawnBullet"))
    {
        auto* shooter = static_cast<GameObject*>(sender);
        auto* otherTag = shooter->GetComponent<dae::TagComponent>();
        auto playerBullet = true;
        
        if (otherTag && otherTag->GetTag() == dae::TagComponent::Tags::Enemy)
            playerBullet = false;

        bool success = false;

        if(playerBullet)
            success = SpawnPlayerBullet(shooter);
        else
            success = SpawnEnemyBullet(shooter);

        //sound here?
        auto& sound = dae::servicelocator::get_sound_system();
        if(success) sound.play(0, 0.5f);
    }
}

bool dae::BulletSpawner::SpawnEnemyBullet(dae::GameObject* shooter)
{
    for (auto* bullet : m_enemyBulletPool)
    {
        if (bullet->IsActive())continue;

        auto pos = shooter->GetTransform().GetWorldPosition();
        bullet->SetPosition(pos.x, pos.y - 20.f);

        auto* velocity = bullet->GetComponent<dae::VelocityComponent>();
        velocity->SetVelocity(0.f,300.f);

        bullet->SetActive(true);

        return true;
    }

    return false;
}

bool dae::BulletSpawner::SpawnPlayerBullet(dae::GameObject* shooter)
{
    for (auto* bullet : m_playerBulletPool)
    {
        if (bullet->IsActive())continue;

        auto pos = shooter->GetTransform().GetWorldPosition();
        bullet->SetPosition(pos.x, pos.y - 20.f);

        auto* velocity = bullet->GetComponent<dae::VelocityComponent>();
        velocity->SetVelocity(0.f, -300.f);

        bullet->SetActive(true);

        return true;
    }

    return false;
}

std::unique_ptr<dae::GameObject> dae::BulletSpawner::CreateBullet(bool isPlayerBullet)
{
    auto bullet = std::make_unique<dae::GameObject>();

    bullet->AddComponent<dae::RenderComponent>()->SetTexture("Images/bullet.png", true);
    bullet->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Bullet);

    auto* collider = bullet->AddComponent<dae::ColliderComponent>(11.f, 25.f);
    auto* damage = bullet->AddComponent<dae::DamageManager>();
    auto* lives = bullet->AddComponent<dae::LivesComponent>(1, dae::LivesComponent::DeathAction::Deactivate);

    bullet->AddComponent<dae::VelocityComponent>(0.f, 0.f);
    bullet->AddComponent<dae::OffscreenDeactivateComponent>(1024.f, 915.f);

    // Observer / Subject
    collider->GetSubject().AddObserver(damage);
    damage->GetSubject().AddObserver(lives);

    if (isPlayerBullet)
        damage->AddThreat(dae::TagComponent::Tags::Enemy);
    else
        damage->AddThreat(dae::TagComponent::Tags::Player);

    // Deactivate Bullet
    bullet->SetActive(false);

    return bullet;
}