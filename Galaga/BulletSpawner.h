#include "Observer.h"
#include "GameObject.h"
#include "Components.h"
#include "Scene.h"

namespace dae
{
    // Engine or Game?
    class BulletSpawner final : public Observer
    {
    public:
        BulletSpawner(Scene& scene) : m_scene(scene) {}

        void Notify(Event event, void* sender) override
        {
            if (event.id == make_sdbm_hash("SpawnBullet"))
            {
                auto* shooter = static_cast<GameObject*>(sender);
                SpawnBullet(shooter);
            }
        }

    private:
        Scene& m_scene;

        void SpawnBullet(GameObject* shooter)
        {
            auto bullet = std::make_unique<GameObject>();

            bullet->AddComponent<RenderComponent>()->SetTexture("bullet.png");

            auto pos = shooter->GetTransform().GetWorldPosition();
            bullet->SetPosition(pos.x, pos.y - 20.f);

            bullet->AddComponent<TagComponent>(dae::TagComponent::Tags::Bullet);
            auto* collider = bullet->AddComponent<ColliderComponent>(10.f, 20.f);
            auto* damage = bullet->AddComponent<dae::DamageManager>();
            auto* lives = bullet->AddComponent<dae::LivesComponent>(1);

            bullet->AddComponent<VelocityComponent>(0.f, -300.f);

            // Observer / Subject
            collider->GetSubject().AddObserver(damage);
            damage->GetSubject().AddObserver(lives);
            damage->AddThreat(dae::TagComponent::Tags::Enemy);

            m_scene.Add(std::move(bullet));
        }
    };
}