#include "Factory.h"

struct EnemyWave
{
    std::vector<glm::vec2> positions;
};

class WaveSpawner
{
public:
    WaveSpawner(dae::Scene& scene)
        : m_scene(scene) {
    }

    void SpawnWave(const EnemyWave& wave)
    {
        for (const auto& pos : wave.positions)
        {
            auto enemy = ActorFactory::CreateEnemy(pos);
            m_scene.Add(std::move(enemy));
        }
    }

private:
    dae::Scene& m_scene;
};