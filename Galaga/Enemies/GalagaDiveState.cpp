#include "GalagaDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"
#include "GameComponents.h"

void dae::GalagaDiveState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();

    auto* enemy = owner->GetComponent<EnemyComponent>();
    if (enemy) enemy->SetDiving(true);

    // screen bounds
    float screenMinX = 100.f;
    float screenMaxX = 900.f;

    // x offset from enemy pos
    float xOffset = (float)(rand() % 241) - 120.f;   // -120 to +120
    float dir = (pos.x < 400.f) ? 1.f : -1.f;

    // dive shape setters
    float width = 180.f + (rand() % 121);   // 180–300
    float height = 450.f + (rand() % 151);  // 450–600
    float skew = (rand() % 61) - 30.f;

    glm::vec2 base = glm::vec2{ pos.x, pos.y };
    float baseX = base.x + xOffset;

    // clamp inside screen
    if (baseX < screenMinX) baseX = screenMinX + 50.f;
    if (baseX > screenMaxX) baseX = screenMaxX - 50.f;

    // height = lowest point
    float lowY = base.y + height;
    float midY = base.y + height * 0.55f;
    float topY = base.y + height * 0.25f;

    // way point generation (hexagon shape)
    m_points.clear();
    m_currentPoint = 0;

    // entry
    m_points.push_back({ baseX,  base.y });

    // top right
    m_points.push_back({ baseX + width * dir + skew, topY });

    // mid right
    m_points.push_back({ baseX + width * 0.6f * dir, midY });

    // bottom
    m_points.push_back({ baseX + skew * 0.2f, lowY });

    // mid left
    m_points.push_back({ baseX - width * 0.5f * dir, midY });

    // top left
    m_points.push_back({ baseX - width * dir - skew, topY });

    // leave screen before returning
    m_points.push_back({ baseX + xOffset * 0.2f, -100.f });

    int bulletRoll = (rand() % 6) - 1;      // 0 - 4
    m_amountBullets = (bulletRoll < 0) ? 0 : bulletRoll;
}

void dae::GalagaDiveState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::GalagaDiveState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_currentPoint >= static_cast<int>(m_points.size()))
        return std::make_unique<EnemyFormationState>(250.f);

    // Shooting mechanic
    if (m_amountBullets > 0)
    {
        m_timer += delta_time;
        if (m_timer >= m_shootingDelay)
        {
            auto* shootComp = owner->GetComponent<dae::ShootComponent>();

            if (shootComp)
            {
                shootComp->Shoot();
                m_amountBullets--;
                m_timer = 0.f;
                // time between bullets
                m_shootingDelay = 0.1f;
            }
        }
    }
    // ---

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

    if (m_segmentSpeed <= 0) m_segmentSpeed = m_speed;

    dir = glm::normalize(dir);
    pos += dir * m_segmentSpeed * delta_time;
    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}
