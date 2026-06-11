#include "GameStatsManager.h"
#include "GameComponents.h"
#include "GameObject.h"
#include <cmath>

void dae::GameStatsManager::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("SpawnBullet"))
    {
        AddShot();
    }
    else if (event.id == make_sdbm_hash("ActorDied"))
    {
        AddHit();
    }
    else if (event.id == make_sdbm_hash("UpdateScore"))
    {
        ScoreComponent* component = static_cast<ScoreComponent*>(sender);
        m_score = component->GetScore();
    }
}

int dae::GameStatsManager::GetHitMissRatio() const
{
    if (m_shotsFired == 0) return 0;

    float ratio = (static_cast<float>(m_hitsScored) / static_cast<float>(m_shotsFired)) * 100.f;
    return static_cast<int>(std::round(ratio));
}
