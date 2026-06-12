#include "BulletSpawner.h"
#include "GameComponents.h"
#include "GameObject.h"
#include "Scene.h"
#include "ServiceLocator.h"

dae::BulletSpawner::BulletSpawner(dae::Scene& scene)
    : m_scene(scene),
    m_subject(this)
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
    if (event.id == make_sdbm_hash("TriggerSpawnBullet"))
    {
        auto* shooter = static_cast<GameObject*>(sender);
        auto* shooterTag = shooter->GetComponent<dae::TagComponent>();
        auto playerBullet = true;
        
        if (shooterTag && shooterTag->GetTag() == dae::TagComponent::Tags::Enemy)
            playerBullet = false;

        bool success = false;

        if(playerBullet)
            success = SpawnPlayerBullet(shooter);
        else
            success = SpawnEnemyBullet(shooter);

        auto& sound = dae::servicelocator::get_sound_system();
        if (success)
        {
            sound.Play(1, 0.5f);
            Event e(make_sdbm_hash("SpawnBullet"));
            m_subject.NotifyObservers(e, this);
        }
    }
}

bool dae::BulletSpawner::SpawnEnemyBullet(dae::GameObject* shooter)
{
    for (auto* bullet : m_enemyBulletPool)
    {
        if (bullet->IsActive())continue;

        auto pos = shooter->GetTransform().GetWorldPosition();
        bullet->SetPosition(pos.x, pos.y + 20.f);

        auto* velocity = bullet->GetComponent<dae::VelocityComponent>();
        velocity->SetVelocity(0.f,300.f);

        if (auto* lives = bullet->GetComponent<dae::LivesComponent>())
        {
            lives->SetLives(1);
        }

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

        if (auto* lives = bullet->GetComponent<dae::LivesComponent>())
        {
            lives->SetLives(1);
        }

        bullet->SetActive(true);

        return true;
    }

    return false;
}

std::unique_ptr<dae::GameObject> dae::BulletSpawner::CreateBullet(bool isPlayerBullet)
{
    auto bullet = std::make_unique<dae::GameObject>();

    glm::vec2 bounds = isPlayerBullet ? glm::vec2{11.f, 25.f} : glm::vec2{ 10.f, 20.f };

    auto* collider = bullet->AddComponent<dae::ColliderComponent>(bounds.x, bounds.y);
    auto* damage = bullet->AddComponent<dae::DamageManager>();
    auto* lives = bullet->AddComponent<dae::LivesComponent>(1, dae::LivesComponent::DeathAction::Deactivate);

    bullet->AddComponent<dae::VelocityComponent>(0.f, 0.f);
    bullet->AddComponent<dae::OffscreenDeactivateComponent>(1024.f, 915.f);

    if (isPlayerBullet)
    {
        bullet->AddComponent<dae::RenderComponent>()->SetTexture("Images/bullet.png", true);
        bullet->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Bullet);
        damage->AddThreat(dae::TagComponent::Tags::Enemy);
    }
    else
    {
        bullet->AddComponent<dae::RenderComponent>()->SetTexture("Images/enemy_bullet.png", true);
        bullet->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::EnemyBullet);
        damage->AddThreat(dae::TagComponent::Tags::Player);
    }

    // Observer / Subject
    damage->AddSubscription(collider->GetSubject().AddObserver(damage));
    lives->AddSubscription(damage->GetSubject().AddObserver(lives));

    // Deactivate Bullet
    bullet->SetActive(false);

    return bullet;
}