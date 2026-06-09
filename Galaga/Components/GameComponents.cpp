#include "GameComponents.h"
#include "GameObject.h"
#include "ServiceLocator.h"
#include "Renderer.h"


#pragma region --- GAME ACTOR ---

void dae::LivesComponent::Notify(Event event, void*)
{
	if (event.id == make_sdbm_hash("TakeDamage"))
	{
		TakeDamage(1);

		// CANVIAR!!!
		if (auto* myTagComp = GetOwner()->GetComponent<TagComponent>(); myTagComp
			&& myTagComp->GetTag() == TagComponent::Tags::Player)
		{
			GetOwner()->SetPosition(450, 600);
		}
	}
}

void dae::LivesComponent::TakeDamage(int damage)
{
	m_lives -= damage;

	if (m_lives <= 0)
	{
		Event e(make_sdbm_hash("ActorDied"));
		m_subject.NotifyObservers(e, GetOwner());

		// Ask!
		GetOwner()->MarkForDestroy();
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

		// Preguntar
		if (!otherTag) return;

		if (otherTag->GetTag() == TagComponent::Tags::Enemy)
		{
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

	//sound here?
	auto& sound = dae::servicelocator::get_sound_system();
	sound.play(0, 0.5f);

	Event e(make_sdbm_hash("SpawnBullet"));
	m_subject.NotifyObservers(e, GetOwner());
}

void dae::DamageManager::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("OnCollision"))
	{
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTagComp = otherObj->GetComponent<TagComponent>();
		auto* myTagComp = GetOwner()->GetComponent<TagComponent>();

		if (!myTagComp || !otherTagComp) return;
		auto otherTag = otherTagComp->GetTag();

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

    for (int i = 0; i < m_lives; ++i)
    {
        Renderer::GetInstance().RenderTexture(*m_texture, pos.x + i * spacing, pos.y);
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
