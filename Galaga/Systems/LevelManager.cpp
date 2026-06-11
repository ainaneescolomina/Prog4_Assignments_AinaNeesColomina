#include "LevelManager.h"
#include "Scene.h"
#include "GameComponents.h"
#include "Factory.h"
#include "EnemyComponents.h"
#include "EnemyFormationState.h"
#include "BulletSpawner.h"
#include "GameStatsManager.h"
#include "GameObject.h"

void dae::LevelManager::Notify(Event event, void* sender)
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

void dae::LevelManager::Update(float delta_time)
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

    if (m_pFormationManager)
    {
        m_pFormationManager->Update(delta_time);
    }
}

void dae::LevelManager::SpawnWave()
{
    if (!m_pScene) return;

    if (!m_pFormationManager)
    {
        m_pFormationManager = std::make_unique<FormationManager>();
        m_pFormationManager->SetEnemies(&m_spawnedEnemies);
    }
    else
        m_pFormationManager->SetActiveFormation(false);

    auto levelPath = "Data/Levels/level" + std::to_string(m_levelIdx) + ".txt";
    m_enemiesToSpawn = LevelLoader::LoadLevel(levelPath);
}

void dae::LevelManager::SkipLevel()
{
    for (auto* enemy : m_spawnedEnemies)
    {
        if (enemy)
            enemy->MarkForDestroy();
    }

    m_spawnedEnemies.clear();

    m_aliveEnemies = 0;
    m_enemiesToSpawn.clear();

    m_levelIdx = (m_levelIdx % 3) + 1;

    SpawnWave();
}

void dae::LevelManager::SpawnEnemy(const EnemySpawnData& enemyData)
{
    auto enemy = ActorFactory::CreateEnemy(enemyData.type, enemyData.pos);

    if (!enemy) return;
    if (enemyData.type == EnemyType::BossGalaga)
    {
        auto beam = ActorFactory::CreateTractorBeam(enemy.get());
        m_pScene->Add(std::move(beam));
    }

    if (auto* lives = enemy->GetComponent<dae::LivesComponent>())
    {
        lives->GetSubject().AddObserver(this);

        if (m_pPlayerScore)
            lives->GetSubject().AddObserver(m_pPlayerScore);

        if(m_pGameStatsManager)
            lives->GetSubject().AddObserver(m_pGameStatsManager);
    }

    if (auto* shoot = enemy->GetComponent<dae::ShootComponent>())
    {
        if (m_pBulletSpawner)
            shoot->GetSubject().AddObserver(m_pBulletSpawner);
    }

    bool fromLeft = rand() % 2 == 0;
    glm::vec2 spawnPos{};

    if (fromLeft)
        spawnPos = {-100.f, enemyData.pos.y - 200.f};
    else
        spawnPos = {1300.f, enemyData.pos.y - 200.f};

    auto stateComp = enemy->GetComponent<EnemyStateComponent>();
    enemy->SetPosition(spawnPos.x, spawnPos.y - 400.f);
    stateComp->SetState(std::make_unique<EnemyFormationState>(215.f));
    
    auto* enemyPtr = enemy.get();
    m_pScene->Add(std::move(enemy));

    ++m_aliveEnemies;
    m_spawnedEnemies.push_back(enemyPtr);
}
