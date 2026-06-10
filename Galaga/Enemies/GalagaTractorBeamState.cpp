#include "GalagaTractorBeamState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"

void dae::GalagaTractorBeamState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = pos;

    m_enemyComp = owner->GetComponent<EnemyComponent>();
    if (m_enemyComp) m_enemyComp->SetDiving(true);

    // screen bounds
    float screenMinX = 100.f;
    float screenMaxX = 900.f;

    // x offset from enemy pos
    float xOffset = (float)(rand() % 241) - 120.f;   // -120 to +120
    float dir = (pos.x < 400.f) ? 1.f : -1.f;

    float loopWidth = 100.f;
    float beamY = 500.f;

    glm::vec2 base = m_startPos;
    float baseX = base.x + xOffset;

    // clamp inside screen
    if (baseX < screenMinX) baseX = screenMinX + 50.f;
    if (baseX > screenMaxX) baseX = screenMaxX - 50.f;

    // way point generation (hexagon shape)
    m_points.clear();
    m_currentPoint = 0;

    // entry
    m_points.push_back({ base.x,  base.y });

    // loop near formation
    m_points.push_back({ pos.x + loopWidth * dir, pos.y + 40.f });
    m_points.push_back({ pos.x - loopWidth * dir, pos.y + 60.f });

    // beam position
    m_points.push_back({ baseX, beamY });

    m_segmentSpeed = m_speed;

    m_beamActive = false;
    m_beamTimer = 0.f;
}

void dae::GalagaTractorBeamState::OnExit(dae::GameObject*)
{
    if (m_enemyComp) m_enemyComp->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::GalagaTractorBeamState::Update(dae::GameObject* owner, float delta_time)
{
    // beam phase
    if (m_beamActive)
    {
        if (m_beamTimer <= 0.1f)
            if (m_enemyComp) m_enemyComp->GetBeamObject()->SetActive(true);

        m_beamTimer += delta_time;

        if (m_beamTimer >= m_beamDuration)
        {
            m_beamActive = false;
            if (m_enemyComp) m_enemyComp->GetBeamObject()->SetActive(false);
            return std::make_unique<EnemyFormationState>(250.f, m_startPos);
        }

        return nullptr;
    }

    if (m_currentPoint >= static_cast<int>(m_points.size()))
    {
        m_beamActive = true;
        return nullptr;
    }

    auto ownerPos = owner->GetTransform().GetPosition();
    glm::vec2 pos{ ownerPos.x, ownerPos.y };
    glm::vec2 target = m_points[m_currentPoint];
    glm::vec2 dir = target - pos;

    float dist = glm::length(dir);

    // reached current waypoint
    if (dist < 10.f)
    {
        ++m_currentPoint;
        m_segmentSpeed = m_speed * (0.7f + (float)(rand() % 61) / 100.f);
        return nullptr;
    }

    dir = glm::normalize(dir);
    pos += dir * m_segmentSpeed * delta_time;
    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}
