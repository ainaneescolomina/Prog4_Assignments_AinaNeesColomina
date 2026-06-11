#pragma once
#include <vector>
#include "LevelLoader.h"
#include "Observer.h"

namespace dae
{
    class Scene;
    class ScoreComponent;
    class GameStatsManager;
    class BulletSpawner;
    class GameObject;

    class WaveSpawner final : public Observer
    {
    public:
        explicit WaveSpawner(dae::Scene& scene) : m_pScene(&scene){}
        void SetPlayerScore(dae::ScoreComponent* score) { m_pPlayerScore = score; }
        void SetBulletSpawner(dae::BulletSpawner* spawner) { m_pBulletSpawner = spawner; }
        void SetGameStats(dae::GameStatsManager* stats) { m_pGameStats = stats; }

        virtual void Notify(Event event, void* sender) override;

        void Update(float delta_time);
        void SpawnWave();

        void SkipLevel();

    private:
        void SpawnEnemy(const EnemySpawnData& enemyData);

        dae::Scene* m_pScene = nullptr;
        dae::ScoreComponent* m_pPlayerScore = nullptr;
        dae::BulletSpawner* m_pBulletSpawner = nullptr;
        dae::GameStatsManager* m_pGameStats = nullptr;

        std::vector<EnemySpawnData> m_enemiesToSpawn;
        std::vector<dae::GameObject*> m_spawnedEnemies;

        int m_levelIdx = 1;
        int m_aliveEnemies{};
    };
}