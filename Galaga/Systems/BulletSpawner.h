#pragma once
#include <memory>
#include <vector>
#include "Observer.h"

namespace dae
{
    class GameObject;
    class Scene;

    class BulletSpawner final : public Observer
    {
    public:
        BulletSpawner(Scene& scene);

        void Notify(Event event, void* sender) override;

    private:
        Scene& m_scene;
        std::vector<GameObject*> m_playerBulletPool;
        std::vector<GameObject*> m_enemyBulletPool;

        const int maxPlayerBullets = 2;
        const int maxEnemyBullets = 15;

        bool SpawnPlayerBullet(GameObject* shooter);
        bool SpawnEnemyBullet(GameObject* shooter);
        std::unique_ptr<dae::GameObject> CreateBullet(bool isPlayerBullet);
    };
}