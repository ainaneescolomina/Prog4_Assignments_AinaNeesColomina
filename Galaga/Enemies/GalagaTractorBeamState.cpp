#include "GalagaTractorBeamState.h"
#include "GameObject.h"
#include "EnemyFormationState.h"
#include "EnemyComponents.h"
#include "GameComponents.h"
#include "ServiceLocator.h"

void dae::GalagaTractorBeamState::OnEnter(dae::GameObject* owner)
{
    auto pos = owner->GetTransform().GetPosition();

    m_enemyComp = owner->GetComponent<EnemyComponent>();
    if (m_enemyComp)
    {
        m_enemyComp->SetDiving(true);
        m_colliderComp = m_enemyComp->GetEnemyOwner()->GetComponent<dae::ColliderComponent>();
    }

    if(m_colliderComp) m_colliderComp->GetSubject().AddObserver(this);

    // screen bounds
    float screenMinX = 100.f;
    float screenMaxX = 900.f;

    // x offset from enemy pos
    float xOffset = (float)(rand() % 241) - 120.f;   // -120 to +120
    float dir = (pos.x < 400.f) ? 1.f : -1.f;

    float loopWidth = 100.f;
    float beamY = 500.f;

    glm::vec2 base = glm::vec2{ pos.x, pos.y };
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
    auto& sound = dae::servicelocator::get_sound_system();
    sound.Stop(5);
    if (m_enemyComp) m_enemyComp->SetDiving(false);
    if (m_colliderComp) m_colliderComp->GetSubject().RemoveObserver(this);
}

std::unique_ptr<dae::EnemyState> dae::GalagaTractorBeamState::Update(dae::GameObject* owner, float delta_time)
{
    if (m_beamSuccessful)
    {
        if (m_enemyComp && m_enemyComp->GetBeamObject())
            m_enemyComp->GetBeamObject()->SetActive(false);
        return std::make_unique<EnemyFormationState>(275.f);
    }

    // beam phase
    if (m_beamActive)
    {
        if (m_beamTimer <= 0.1f)
        {
            if (m_enemyComp && !m_enemyComp->GetBeamObject()->IsActive())
            {
                auto& sound = dae::servicelocator::get_sound_system();
                sound.Play(5, 0.5f);
                m_enemyComp->GetBeamObject()->SetActive(true);
            }
        }

        m_beamTimer += delta_time;

        if (m_beamTimer >= m_beamDuration)
        {
            m_beamActive = false;
            if (m_enemyComp)
            {
                m_enemyComp->GetBeamObject()->SetActive(false);
            }
            return std::make_unique<EnemyFormationState>(250.f);
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

void dae::GalagaTractorBeamState::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("OnCollision"))
    {
        auto* otherObj = static_cast<GameObject*>(sender);
        auto* otherTagComp = otherObj->GetComponent<dae::TagComponent>();

        if (!otherTagComp) return;
        auto otherTag = otherTagComp->GetTag();

        // Check if Tractor Beam
        if (otherTag == TagComponent::Tags::Player)
            m_beamSuccessful = true;
    }
}
