#pragma once
#include <vector>
#include "LevelLoader.h"
#include "Observer.h"

namespace dae
{
    class Scene;
    class ScoreComponent;

    class WaveSpawner final : public Observer
    {
    public:
        explicit WaveSpawner(dae::Scene& scene) : m_pScene(&scene){}
        void SetPlayerScore(dae::ScoreComponent* score) { m_pPlayerScore = score; }

        virtual void Notify(Event event, void* sender) override;

        void Update(float delta_time);
        void SpawnWave();

    private:
        void SpawnEnemy(const EnemySpawnData& enemyData);

        dae::Scene* m_pScene = nullptr;
        dae::ScoreComponent* m_pPlayerScore = nullptr;
        std::vector<EnemySpawnData> m_enemiesToSpawn;

        int m_levelIdx = 1;
        int m_aliveEnemies{};
    };
}