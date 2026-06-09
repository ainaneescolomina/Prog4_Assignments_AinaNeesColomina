#include "WaveSpawner.h"
#include "Scene.h"
#include "GameComponents.h"
#include "Factory.h"
#include "EnemyComponents.h"
#include "EnemyFormationState.h"

void dae::WaveSpawner::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("ActorDied"))
    {
        auto* otherObj = static_cast<GameObject*>(sender);
        auto* otherTag = otherObj->GetComponent<TagComponent>();

        if (!otherTag) return;
        if (otherTag->GetTag() == TagComponent::Tags::Enemy)
        {
            --m_aliveEnemies;

            if (m_aliveEnemies <= 0)
            {
                m_levelIdx = (m_levelIdx % 3) + 1;
                SpawnWave();
            }
        }
    }
}

void dae::WaveSpawner::Update(float delta_time)
{
    for (auto& enemy :
        m_enemiesToSpawn)
    {
        enemy.spawnDelay -= delta_time;
    }

    while (!m_enemiesToSpawn.empty() && m_enemiesToSpawn.front().spawnDelay <= 0.f)
    {
        SpawnEnemy(m_enemiesToSpawn.front());
        m_enemiesToSpawn.erase(m_enemiesToSpawn.begin());
    }
}

void dae::WaveSpawner::SpawnWave()
{
    if (!m_pScene) return;
    auto levelPath = "Data/Levels/level" + std::to_string(m_levelIdx) + ".txt";
    m_enemiesToSpawn = LevelLoader::LoadLevel(levelPath);
}

void dae::WaveSpawner::SpawnEnemy(const EnemySpawnData& enemyData)
{
    auto enemy = ActorFactory::CreateEnemy(enemyData.type, enemyData.pos);

    if (!enemy) return;
    if (auto* lives = enemy->GetComponent<dae::LivesComponent>())
    {
        lives->GetSubject().AddObserver(this);

        if (m_pPlayerScore)
            lives->GetSubject().AddObserver(m_pPlayerScore);
    }

    bool fromLeft = rand() % 2 == 0;
    glm::vec2 spawnPos{};

    if (fromLeft)
        spawnPos = {-100.f, enemyData.pos.y - 200.f};
    else
        spawnPos = {1300.f, enemyData.pos.y - 200.f};

    auto stateComp = enemy->GetComponent<EnemyStateComponent>();
    enemy->SetPosition(spawnPos.x, spawnPos.y - 400.f);
    stateComp->SetState(std::make_unique<EnemyFormationState>(215.f, enemyData.pos));
    m_pScene->Add(std::move(enemy));

    ++m_aliveEnemies;
}
