#include "LevelManager.h"
#include "Scene.h"
#include "GameComponents.h"
#include "Factory.h"
#include "EnemyComponents.h"
#include "EnemyEntranceState.h"
#include "BulletSpawner.h"
#include "GameStatsManager.h"
#include "GameObject.h"

// Listens for enemy deaths to check if the next level needs to be spawned
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

void dae::LevelManager::AddSubscription(dae::Subscription subscription)
{
    m_subscriptions.emplace_back(std::move(subscription));
}

// Spawns enemies keeping their delay on account
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

    if (m_pFormationManager && m_enemiesToSpawn.empty())
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

// Empties the current level to trigger an instant level-skip
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
    // Skip creating a Boss if a live player controls them inside Versus mode
    if (enemyData.type == EnemyType::BossGalaga && m_isVersusMode) return;
    auto enemy = ActorFactory::CreateEnemy(enemyData.type, enemyData.pos);

    if (!enemy) return;
    if (enemyData.type == EnemyType::BossGalaga)
    {
        auto beam = ActorFactory::CreateTractorBeam(enemy.get());
        m_pScene->Add(std::move(beam));
    }

    if (auto* lives = enemy->GetComponent<dae::LivesComponent>())
    {
        this->AddSubscription(lives->GetSubject().AddObserver(this));
        

        if (m_pPlayerScore)
            m_pPlayerScore->AddSubscription(lives->GetSubject().AddObserver(m_pPlayerScore));

        if(m_pGameStatsManager)
            m_pGameStatsManager->AddSubscription(lives->GetSubject().AddObserver(m_pGameStatsManager));
    }

    if (auto* shoot = enemy->GetComponent<dae::ShootComponent>())
    {
        if (m_pBulletSpawner)
            m_pBulletSpawner->AddSubscription(shoot->GetSubject().AddObserver(m_pBulletSpawner));
    }

    bool fromLeft = rand() % 2 == 0;
    glm::vec2 spawnPos{};

    if (fromLeft)
        spawnPos = {-100.f, enemyData.pos.y - 200.f};
    else
        spawnPos = {1300.f, enemyData.pos.y - 200.f};

    // Passes fixed loop coordinate coordinates to steer entrance animations
    std::vector<glm::vec2> customPath = GetEntrancePath(enemyData.row, enemyData.pos);
    enemy->SetPosition(customPath[0].x, customPath[0].y);

    auto stateComp = enemy->GetComponent<EnemyStateComponent>();
    stateComp->SetState(std::make_unique<EnemyEntranceState>(550.f, customPath));
    
    auto* enemyPtr = enemy.get();
    m_pScene->Add(std::move(enemy));

    ++m_aliveEnemies;
    m_spawnedEnemies.push_back(enemyPtr);
}

// DESIGN CHOICE: Hardcoded structural waypoint paths
std::vector<glm::vec2> dae::LevelManager::GetEntrancePath(int row, const glm::vec2& targetGridPos)
{
    std::vector<glm::vec2> entrancePath{};
    // Different entrance path based on the row
    int pathChoice = row % 3;

    switch (pathChoice)
    {
    case 0: // Left side loop
        entrancePath = {
                    { -50.f, 950.f }, { 150.f, 500.f }, { 200.f, 400.f }, { 275.f, 375.f },
                    { 350.f, 400.f }, { 375.f, 475.f }, { 350.f, 550.f }, { 275.f, 575.f },
                    { 200.f, 550.f }, { 175.f, 475.f }, { 250.f, 350.f }, targetGridPos
        };
        break;

    case 1: // Right side loop
        entrancePath = {
                    { 1050.f, 950.f }, { 850.f, 500.f }, { 800.f, 400.f }, { 725.f, 375.f },
                    { 650.f, 400.f }, { 625.f, 475.f }, { 650.f, 550.f }, { 725.f, 575.f },
                    { 800.f, 550.f }, { 825.f, 475.f }, { 750.f, 350.f }, targetGridPos
        };
        break;

    case 2: // Top left to center loop
    default:
        entrancePath = {
                    { -50.f, -50.f }, { 250.f, 250.f }, { 450.f, 350.f }, { 550.f, 350.f },
                    { 600.f, 400.f }, { 600.f, 500.f }, { 550.f, 550.f }, { 450.f, 550.f },
                    { 400.f, 475.f }, { 500.f, 300.f }, targetGridPos
        };
        break;
    }

    return entrancePath;
}
