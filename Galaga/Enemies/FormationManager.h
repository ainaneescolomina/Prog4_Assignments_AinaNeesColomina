#pragma once
#include <vector>

namespace dae
{
    class GameObject;

    // DESCRIPTION: Coordinates the layout logic for the enemies. 
    // Manages idle offsets and uses dynamic cooldown intervals to trigger "smart" dive attacks
    class FormationManager final
    {
    public:
        FormationManager() = default;
        ~FormationManager() = default;

        void Update(float deltaTime);

        bool GetActiveFormation() const { return m_activeFormation; }

        // Stores a refereence to the active enemy list maintained by the Level Manager
        void SetEnemies(const std::vector<GameObject*>* enemies) { m_pSpawnedEnemies = enemies; }
        void SetActiveFormation(bool active) { m_activeFormation = active; }

    private:
        void TriggerDiveWave();
        void TriggerDiveState(GameObject* chosenEnemy);

        const std::vector<GameObject*>* m_pSpawnedEnemies = nullptr;

        // Idle Breathing variables
        float m_idleMovementTimer{ 0.f };

        // Smart Dive Timers
        bool m_activeFormation{ false };
        float m_diveCooldownTimer{ 0.f };
        float m_timeBetweenDives{ 2.75f };
    };
}