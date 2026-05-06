#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace dae
{
    class Scene;
    class ScoreComponent;

    struct EnemyWave
    {
        std::vector<glm::vec2> positions;
    };

    class WaveSpawner
    {
    public:
        explicit WaveSpawner(dae::Scene& scene) : m_pScene(&scene) {}

        void SpawnWave(const EnemyWave& wave, dae::ScoreComponent* playerScore);

    private:
        dae::Scene* m_pScene = nullptr;
    };
}