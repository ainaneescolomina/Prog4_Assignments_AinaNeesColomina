#include "GameComponents.h"
#include "GameObject.h"
#include "Renderer.h"
#include "EnemyComponents.h"


#pragma region --- GAME ACTOR ---

void dae::MovementComponent::Notify(Event event, void*)
{
	if (event.id == make_sdbm_hash("TakeDamage"))
	{
		m_isExploding = true;
		m_explodingTimer = 0.8f;
		m_isFrozen = true;

		GetOwner()->RemoveComponent<TractorBeamTargetComponent>();
		GetOwner()->GetComponent<dae::AnimatedRenderComponent>()->SetTexture("Images/player_explosion.png", 1, 4, 0.2f, true);
	}
	else if (event.id == make_sdbm_hash("FreezePosition"))
	{
		m_isFrozen = true;
	}
}

void dae::MovementComponent::Update(float delta_time)
{
	if (!m_isExploding) return;
	m_explodingTimer -= delta_time;

	if (m_explodingTimer <= 0)
	{
		GetOwner()->SetPosition(m_resetPos.x, m_resetPos.y);
		GetOwner()->GetComponent<dae::AnimatedRenderComponent>()->SetTexture("Images/player.png", 1, 1, 0.f, true);
		m_isExploding = false;
		m_isFrozen = false;
	}
}


void dae::LivesComponent::Notify(Event event, void*)
{
	if (event.id == make_sdbm_hash("TakeDamage"))
	{
		if (m_timer > 0.f) return;
		m_timer = m_cooldown;

		TakeDamage(1);
	}
}

void dae::LivesComponent::Update(float delta_time)
{
	m_timer -= delta_time;
}

void dae::LivesComponent::TakeDamage(int damage)
{
	m_lives -= damage;

	if (m_lives <= 0)
	{
		Event e(make_sdbm_hash("ActorDied"));
		m_subject.NotifyObservers(e, GetOwner());

		switch (m_deathAction)
		{
		case dae::LivesComponent::DeathAction::Destroy:
			GetOwner()->MarkForDestroy();
			break;
		case dae::LivesComponent::DeathAction::Deactivate:
			GetOwner()->SetActive(false);
			break;
		case dae::LivesComponent::DeathAction::None:
		default:
			break;
		}			
	}

	Event e(make_sdbm_hash("UpdateLives"));
	m_subject.NotifyObservers(e);
}

void dae::ScoreComponent::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("ActorDied"))
	{
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTag = otherObj->GetComponent<TagComponent>();
		auto* otherEnemy = otherObj->GetComponent<EnemyComponent>();

		// Preguntar
		if (!otherTag) return;
		if (otherTag->GetTag() == TagComponent::Tags::Enemy)
		{
			if (otherEnemy)
				AddScore(otherEnemy->GetScoreValue());
			else
				AddScore(100);
		}
	}
}

void dae::ScoreComponent::AddScore(int score)
{
	m_score += score;

	Event e(make_sdbm_hash("UpdateScore"));
	m_subject.NotifyObservers(e);
}

void dae::ShootComponent::Update(float delta_time)
{
	m_timer -= delta_time;
}

void dae::ShootComponent::Shoot()
{
	if (m_timer > 0.f) return;

	m_timer = m_cooldown;

	Event e(make_sdbm_hash("TriggerSpawnBullet"));
	m_subject.NotifyObservers(e, GetOwner());
}

void dae::DamageManager::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("OnCollision"))
	{
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTagComp = otherObj->GetComponent<TagComponent>();
		auto* myTagComp = GetOwner()->GetComponent<TagComponent>();
		auto* myBeamTargetComp = GetOwner()->GetComponent<TractorBeamTargetComponent>();

		if (!myTagComp || !otherTagComp || myBeamTargetComp) return;
		auto otherTag = otherTagComp->GetTag();

		// Check if Tractor Beam
		if (otherTag == TagComponent::Tags::TractorBeam && otherObj->IsActive()
			&& myTagComp->GetTag() == TagComponent::Tags::Player)
		{
			HandleTractorBeamCapture(otherObj);
			return;
		}

		// Normal Damage Loops
		for (auto threat : m_threats)
		{
			if (threat != otherTag) continue;
			Event e(make_sdbm_hash("TakeDamage"));
			m_subject.NotifyObservers(e);
		}
	}
}

void dae::DamageManager::AddThreat(dae::TagComponent::Tags threat)
{
	m_threats.emplace_back(threat);
}

void dae::DamageManager::HandleTractorBeamCapture(GameObject* beamObject)
{
	auto* moveComp = GetOwner()->GetComponent<MovementComponent>();
	if (moveComp)
		moveComp->SetIsFrozen(true);

	if (GetOwner()->GetComponent<TractorBeamTargetComponent>() == nullptr)
	{
		auto targetLocation = beamObject->GetTransform().GetWorldPosition();
		auto* beamTarget = GetOwner()->AddComponent<TractorBeamTargetComponent>(targetLocation.x, targetLocation.y - 20.f);

		auto* livesComp = GetOwner()->GetComponent<LivesComponent>();
		if (livesComp) beamTarget->GetSubject().AddObserver(livesComp);
		beamTarget->GetSubject().AddObserver(moveComp);
	}
}

void dae::ScreenBoundsComponent::Update(float)
{
	auto pos = GetOwner()->GetTransform().GetWorldPosition();

	// Clamp the position within the set bounding box
	float clampedX = std::clamp(pos.x, m_minX, m_maxX);
	float clampedY = std::clamp(pos.y, m_minY, m_maxY);

	// Change the position if it's outside the bounds
	if (clampedX != pos.x || clampedY != pos.y)
		GetOwner()->SetPosition(clampedX, clampedY);
}

void dae::OffscreenDeactivateComponent::Update(float)
{
	const auto pos = GetOwner()->GetTransform().GetWorldPosition();

	if (pos.x < 20.f || pos.x > m_width ||
		pos.y < 20.f || pos.y > m_height)
	{
		GetOwner()->SetActive(false);
	}
}

void dae::TractorBeamTargetComponent::Update(float delta_time)
{
	glm::vec2 playerPos = GetOwner()->GetTransform().GetPosition();

	float pullSpeed = 120.f;

	if (playerPos.y > (m_targetPos.y - 100.f))
	{
		// move toward the top of the beam
		float xDiff = m_targetPos.x - playerPos.x;
		float driftSpeed = 2.5f;

		GetOwner()->SetPosition(
			playerPos.x + (xDiff * delta_time * driftSpeed),
			playerPos.y - (delta_time * pullSpeed)
		);
	}
	else
	{
		Event e(make_sdbm_hash("TakeDamage"));
		m_subject.NotifyObservers(e);
	}
}

#pragma endregion

#pragma region --- UI ---

void dae::LivesDisplayComponent::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("UpdateLives"))
    {
        LivesComponent* component = static_cast<LivesComponent*>(sender);
        m_lives = component->GetLives();
    }
}

void dae::LivesDisplayComponent::Render() const
{
    if (m_texture == nullptr) return;
    const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
    const float spacing = 65.f;

    for (int i = 1; i < m_lives; ++i)
    {
        Renderer::GetInstance().RenderTexture(*m_texture, pos.x + (i-1) * spacing, pos.y);
    }
}

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* ownerRef, std::shared_ptr<Font> font, const SDL_Color& color)
    : TextComponent(ownerRef, "", font, color)
{
    UpdateText();
}

void dae::ScoreDisplayComponent::Notify(Event event, void* sender)
{
    if (event.id == make_sdbm_hash("UpdateScore"))
    {
        ScoreComponent* component = static_cast<ScoreComponent*>(sender);
        m_score = component->GetScore();

        UpdateText();
    }
}

void dae::ScoreDisplayComponent::UpdateText()
{
    std::string text = std::to_string(m_score);
    SetText(text);
}

#pragma endregion
