#include "WaveSpawner.h"
#include "Scene.h"
#include "GameComponents.h"
#include "Factory.h"
#include "EnemyComponents.h"
#include "EnemyFormationState.h"
#include "LevelLoader.h"

void dae::WaveSpawner::SpawnWave(dae::ScoreComponent* playerScore)
{
    if (!m_pScene) return;
    auto levelPath = "Data/Levels/level" + std::to_string(m_levelIdx) + ".txt";
    auto levelData = LevelLoader::LoadLevel(levelPath);

    for (const auto& enemyData : levelData)
    {
        auto enemy = ActorFactory::CreateEnemy(enemyData.type, enemyData.pos);

        if (!enemy) continue;
        if (playerScore)
        {
            if (auto* lives = enemy->GetComponent<dae::LivesComponent>())
                lives->GetSubject().AddObserver(playerScore);
        }

        auto stateComp = enemy->GetComponent<EnemyStateComponent>();
        enemy->SetPosition(enemyData.pos.x, -50.f);
        stateComp->SetState(std::make_unique<EnemyFormationState>(175.f, enemyData.pos));
        m_pScene->Add(std::move(enemy));
    }

    ++m_levelIdx;
}