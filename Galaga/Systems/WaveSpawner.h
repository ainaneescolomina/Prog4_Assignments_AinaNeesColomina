#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace dae
{
    class Scene;
    class ScoreComponent;

    class WaveSpawner final
    {
    public:
        explicit WaveSpawner(dae::Scene& scene) : m_pScene(&scene) {}

        void SpawnWave(dae::ScoreComponent* playerScore);

    private:
        dae::Scene* m_pScene = nullptr;
        int m_levelIdx = 1;
    };
}