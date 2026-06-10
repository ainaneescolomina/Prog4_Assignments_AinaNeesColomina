#include "ButterflyDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"

void dae::ButterflyDiveState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = glm::vec2{ pos.x, pos.y };

    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(true);
    
    // screen bounds
    float screenMinX = 100.f;
    float screenMaxX = 900.f;

    // x offset from enemy pos
    float xOffset = (float)(rand() % 161) - 80.f;
    float dir = (rand() % 2 == 0) ? 1.f : -1.f;

    // dive shape setters
    float width = 90.f + (rand() % 90);
    float height = 375.f + (rand() % 260);
    float skew = (rand() % 61) - 30.f;

    glm::vec2 base = m_startPos;
    float baseX = base.x + xOffset;

    // clamp inside screen
    if (baseX < screenMinX) baseX = screenMinX + 50.f;
    if (baseX > screenMaxX) baseX = screenMaxX - 50.f;

    float lowY = base.y + height;
    float midY = base.y + height * 0.5f;
    float topY = base.y + height * 0.2f;

    // way point generation (S shape)
    m_points.clear();
    m_currentPoint = 0;

    // entry
    m_points.push_back({ baseX, base.y });

    // zig
    m_points.push_back({baseX + (width * 0.6f * dir) + skew, topY});

    // zag
    m_points.push_back({baseX - (width * 0.4f * dir) - skew * 0.5f, midY});

    // bottom
    m_points.push_back({baseX + skew * 0.3f, lowY});

    // second zig
    m_points.push_back({baseX - (width * 0.7f * dir), midY * 0.9f});

    // return
    m_points.push_back({baseX + xOffset * 1.5f, base.y + 75.f});
}

void dae::ButterflyDiveState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::ButterflyDiveState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_currentPoint >= m_points.size())
        return std::make_unique<EnemyFormationState>(250.f, m_startPos);

    auto pos3D = owner->GetTransform().GetPosition();
    glm::vec2 pos{ pos3D.x, pos3D.y };

    glm::vec2 target = m_points[m_currentPoint];
    glm::vec2 dir = target - pos;

    float dist = glm::length(dir);

    if (dist < 10.f)
    {
        ++m_currentPoint;
        m_segmentSpeed = m_speed * (0.8f + (float)(rand() % 41) / 100.f);
        return nullptr;
    }

    if (m_segmentSpeed <= 0) m_segmentSpeed = m_speed;

    dir = glm::normalize(dir);
    pos += dir * m_segmentSpeed * delta_time;

    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}
