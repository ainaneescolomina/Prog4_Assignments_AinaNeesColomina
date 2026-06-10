#include "EnemyDiveState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"
#include <random>

void dae::EnemyDiveState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();
    m_startPos = pos;

    auto* enemy = owner->GetComponent<EnemyComponent>();
    if (enemy) enemy->SetDiving(true);

    // -----------------------------
    // controlled randomness
    // -----------------------------
    m_xOffset = (float)(rand() % 241) - 120;   // -120 to +120
    m_sway = (float)(rand() % 81) - 40;    // -40 to +40

    float side = (pos.x < 400.f) ? 1.f : -1.f;

    float width = 120.f + (rand() % 121);   // 120–240
    float height = 160.f + (rand() % 161);   // 160–320
    float skew = (rand() % 81) - 40.f;     // -40 to +40
    float asymmetry = (rand() % 41) - 20.f;     // -20 to +20

    glm::vec2 base = m_startPos;
    // helper offsets
    float topY = height * 0.2f;
    float midY = height * 0.6f;
    float bottomY = height * 1.0f;

    // -----------------------------
    // hex-ish dive with offset
    // -----------------------------
    m_points.clear();
    m_currentPoint = 0;

    m_points.push_back(base);

    // top right
    m_points.push_back({
        base.x + width * side + skew,
        base.y + topY
        });

    // mid right
    m_points.push_back({
        base.x + width * side + skew * 0.5f,
        base.y + midY
        });

    // bottom center (deep dive)
    m_points.push_back({
        base.x + skew * 0.2f,
        base.y + bottomY + asymmetry
        });

    // mid left
    m_points.push_back({
        base.x - width * side - skew * 0.5f,
        base.y + midY
        });

    // top left
    m_points.push_back({
        base.x - width * side - skew,
        base.y + topY
        });

    // return
    m_points.push_back(base);
}

void dae::EnemyDiveState::OnExit(dae::GameObject* owner)
{
    auto* enemy = owner->GetComponent<dae::EnemyComponent>();
    if (enemy) enemy->SetDiving(false);
}

std::unique_ptr<dae::EnemyState> dae::EnemyDiveState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_currentPoint >= m_points.size())
    {
        return std::make_unique<
            EnemyFormationState>(
                250.f,
                m_startPos);
    }

    auto p =
        owner->GetTransform().GetPosition();

    glm::vec2 pos{ p.x, p.y };

    glm::vec2 target =
        m_points[m_currentPoint];

    glm::vec2 dir =
        target - pos;

    float dist = glm::length(dir);

    // reached current waypoint
    if (dist < 10.f)
    {
        ++m_currentPoint;
        return nullptr;
    }

    dir = glm::normalize(dir);

    pos += dir * m_speed * delta_time;

    owner->SetPosition(pos.x, pos.y);

    return nullptr;
}