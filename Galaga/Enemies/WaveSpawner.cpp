#include "WaveSpawner.h"
#include "Scene.h"
#include "Components.h"
#include "Factory.h"
#include "EnemyStateComponent.h"
#include "EnemyFormationState.h"

void dae::WaveSpawner::SpawnWave(const dae::EnemyWave& wave, dae::ScoreComponent* playerScore)
{
    if (!m_pScene) return;

    for (const auto& pos : wave.positions)
    {
        auto enemy = ActorFactory::CreateEnemy(pos);

        if (enemy)
        {
            if (playerScore)
            {
                if (auto* lives = enemy->GetComponent<dae::LivesComponent>())
                    lives->GetSubject().AddObserver(playerScore);
            }

            auto stateComp = enemy->GetComponent<EnemyStateComponent>();
            enemy->SetPosition(pos.x, -50.f);
            stateComp->SetState(std::make_unique<EnemyFormationState>(175.f, pos));
            m_pScene->Add(std::move(enemy));
        }
    }
}