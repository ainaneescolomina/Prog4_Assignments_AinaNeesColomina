#include "BeeDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"
#include <random>

void dae::BeeDiveState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = pos;

    auto* enemy = owner->GetComponent<EnemyComponent>();
    if (enemy) enemy->SetDiving(true);

    // screen bounds
    float screenMinX = 100.f;
    float screenMaxX = 900.f;

    // x offset from enemy pos
    float xOffset = (float)(rand() % 241) - 120.f;   // -120 to +120
    float dir = (pos.x < 400.f) ? 1.f : -1.f;

    // dive shape setters
    float width = 150.f + (rand() % 151);       // 150–300
    float height = 350.f + (rand() % 251);      // 350–600
    float skew = (rand() % 81) - 40.f;          // -40 to +40
    float asymmetry = (rand() % 41) - 20.f;     // -20 to +20

    // height = center of the dive shape
    // make height affect horizontal spread
    float widthScale = 0.6f + (height / 1200.f);
    width *= widthScale;

    glm::vec2 base = m_startPos;
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
    m_points.push_back({ baseX + width * dir + skew * 0.5f, midY });

    // bottom
    m_points.push_back({ baseX + skew * 0.2f, lowY + asymmetry });

    // mid left
    m_points.push_back({ baseX - width * dir - skew * 0.5f, midY });

    // top left
    m_points.push_back({ baseX - width * dir - skew, topY });

    // return
    m_points.push_back({
    baseX + xOffset * 0.2f,
    base.y + 50.f   // direction of the "escape"
        });
}

void dae::BeeDiveState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::BeeDiveState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_currentPoint >= m_points.size())
        return std::make_unique<EnemyFormationState>(250.f,m_startPos);

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