#include "FormationManager.h"
#include "GameObject.h"
#include "EnemyComponents.h"

#include "EnemyIdleState.h"

#include "BeeDiveState.h"
#include "ButterflyDiveState.h"
#include "GalagaDiveState.h"
#include "GalagaTractorBeamState.h"

#include <cmath>

void dae::FormationManager::Update(float deltaTime)
{
    if (!m_pSpawnedEnemies || m_pSpawnedEnemies->empty()) return;

    // Halts dive cycles until all enemies finish their entrance
    if (!m_activeFormation)
    {
        bool allEnemiesIdle = true;
        for (auto* enemy : *m_pSpawnedEnemies)
        {
            if (!enemy) continue;
            auto* stateComp = enemy->GetComponent<EnemyStateComponent>();
            if (!stateComp) continue;

            if (stateComp->GetCurrentState()->GetType() != EnemyStateType::Idle)
            {
                allEnemiesIdle = false;
                break;
            }
        }

        if (allEnemiesIdle) m_activeFormation = true;
    }

    if (!m_activeFormation) return;

    // Idle formation movement
    m_idleMovementTimer += deltaTime;
    float lateralOffset = std::sin(m_idleMovementTimer * 1.0f) * 20;

    for (auto* enemy : *m_pSpawnedEnemies)
    {
        if (!enemy || enemy->IsMarkedForDestroy()) continue;
        auto* stateComp = enemy->GetComponent<EnemyStateComponent>();
        if (!stateComp) continue;

        auto* currentState = stateComp->GetCurrentState();

        if (currentState->GetType() == EnemyStateType::Idle)
        {
            auto* idleState = static_cast<dae::EnemyIdleState*>(currentState);
            idleState->SetFormationOffset(lateralOffset);
        }
    }

    // Dive pattern calculation
    m_diveCooldownTimer += deltaTime;
    if (m_diveCooldownTimer >= m_timeBetweenDives)
    {
        m_diveCooldownTimer = 0.f;
        m_timeBetweenDives = 2.5f + (static_cast<float>(rand() % 200) / 100.f);
        TriggerDiveWave();
    }
}

void dae::FormationManager::TriggerDiveWave()
{
    int currentDivers = 0;
    std::vector<GameObject*> readyEnemies;

    // Filters and counts current diving behavior statuses across the list
    for (auto* enemy : *m_pSpawnedEnemies)
    {
        if (!enemy) continue;
        auto* stateComp = enemy->GetComponent<EnemyStateComponent>();
        if (!stateComp) continue;

        EnemyStateType stateType = stateComp->GetCurrentState()->GetType();

        if (stateType == EnemyStateType::Dive)
        {
            currentDivers++;
        }
        else if (stateType == EnemyStateType::Idle)
        {
            readyEnemies.push_back(enemy);
        }
    }

    if (currentDivers >= 3 || readyEnemies.empty()) return;

    GameObject* chosenEnemy = nullptr;
    int chosenIndex = -1;

    // Prioritize Boss Galaga
    for (auto* enemy : readyEnemies)
    {
        auto* enemyComp = enemy->GetComponent<EnemyComponent>();
        if (enemyComp && enemyComp->GetType() == EnemyType::BossGalaga)
        {
            if (rand() % 100 < 40) { chosenEnemy = enemy; break; }
        }
    }

    // Fallback logic: Select a random enemy if the Boss pass fails or is unavailable
    if (!chosenEnemy)
    {
        chosenIndex = rand() % static_cast<int>(readyEnemies.size());
        chosenEnemy = readyEnemies[chosenIndex];
    }

    TriggerDiveState(chosenEnemy);

    // CO-OP DIVE: 55% chance to trigger an adjacent enemy unit for a dual-dive wave
    if (currentDivers < 2 && (rand() % 100 < 55))
    {
        GameObject* adjacentEnemy = nullptr;

        if (chosenIndex > 0 && (rand() % 2 == 0))
            adjacentEnemy = readyEnemies[chosenIndex - 1];
        else if (chosenIndex < static_cast<int>(readyEnemies.size()) - 1)
            adjacentEnemy = readyEnemies[chosenIndex + 1];

        if (adjacentEnemy)
        {
            // check to make sure the neighbor isn't a Boss
            auto* neighborEnemyComp = adjacentEnemy->GetComponent<EnemyComponent>();
            if (neighborEnemyComp && neighborEnemyComp->GetType() != EnemyType::BossGalaga)
            {
                TriggerDiveState(adjacentEnemy);
            }
        }
    }
}

void dae::FormationManager::TriggerDiveState(GameObject* chosenEnemy)
{
    auto* stateComp = chosenEnemy->GetComponent<EnemyStateComponent>();
    auto* enemyComp = chosenEnemy->GetComponent<EnemyComponent>();

    if (stateComp && enemyComp)
    {
        std::unique_ptr<EnemyState> diveState = nullptr;

        switch (enemyComp->GetType())
        {
        case EnemyType::Bee:
            diveState = std::make_unique<BeeDiveState>();
            break;

        case EnemyType::Butterfly:
            diveState = std::make_unique<ButterflyDiveState>();
            break;

        case EnemyType::BossGalaga:
        {
            bool tractorBeam = (rand() % 100) < 30;
            if (tractorBeam)
                diveState = std::make_unique<GalagaTractorBeamState>();
            else
                diveState = std::make_unique<GalagaDiveState>();
            break;
        }
        }

        if (diveState)
        {
            stateComp->SetState(std::move(diveState));
        }
    }
}
