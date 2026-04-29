#include "Components.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/gtc/constants.hpp>
#include "Renderer.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h"

#include "ServiceLocator.h"

#pragma region --- BASE ---

void dae::RenderComponent::Render() const
{
	if (m_texture == nullptr) return;
	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextComponent::Update(float)
{
	if (m_needsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);
		m_texture = std::make_shared<Texture2D>(texture);
		m_needsUpdate = false;
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_color = color;
	m_needsUpdate = true;
}

dae::FPSComponent::FPSComponent(GameObject* ownerRef)
	:Component(ownerRef)
{
	m_pTextComponent = GetOwner()->GetComponent<TextComponent>();
}

void dae::FPSComponent::Update(float delta_time)
{
	m_elapsedTime += delta_time;
	m_frameCount++;

	if (m_elapsedTime >= 1.0f)
	{
		auto newFps = static_cast<float>(m_frameCount) / m_elapsedTime;

		if (std::fabs(m_fps - newFps) >= 0.01f)
		{
			m_fps = newFps;
			if (m_pTextComponent)
			{
				std::ostringstream ss;
				ss << std::fixed << std::setprecision(2) << m_fps;
				m_pTextComponent->SetText(ss.str() + " FPS");
			}
		}

		// Reset counters
		m_elapsedTime = 0.0f;
		m_frameCount = 0;
	}
}

float dae::FPSComponent::GetFPS() const
{
	return m_fps;
}

dae::RotatorComponent::RotatorComponent(GameObject* ownerRef, float radius, float speed)
	: Component(ownerRef),
	m_radius(radius),
	m_speed(speed)
{
	m_startPosition = GetOwner()->GetTransform().GetPosition();
}

void dae::RotatorComponent::Update(float deltaTime)
{
    m_angle += m_speed * deltaTime;
    m_angle = std::fmod(m_angle, glm::two_pi<float>());

    float offsetX = cos(m_angle) * m_radius;
    float offsetY = sin(m_angle) * m_radius;

    glm::vec3 center{};

    if (auto parent = GetOwner()->GetParent())
        center = parent->GetTransform().GetWorldPosition();
    else
        center = GetOwner()->GetTransform().GetWorldPosition();

    GetOwner()->SetPosition(center.x + offsetX,
                            center.y + offsetY);
}

#pragma endregion

#pragma region --- GAME ACTOR ---

void dae::LivesComponent::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("OnCollision"))
	{
		//auto* otherCollider = static_cast<ColliderComponent*>(sender);
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTagComp = otherObj->GetComponent<TagComponent>();
		auto* myTagComp = GetOwner()->GetComponent<TagComponent>();

		// Preguntar
		if (!myTagComp || !otherTagComp) return;
		auto myTag = myTagComp->GetTag();
		auto otherTag = otherTagComp->GetTag();

		if (myTag == TagComponent::Tags::Player && otherTag == TagComponent::Tags::Enemy)
		{
			GetOwner()->SetPosition(450.f, 400.f);
			TakeDamage(1);
		}
		else if ((myTag == TagComponent::Tags::Bullet && otherTag == TagComponent::Tags::Enemy) ||
			(myTag == TagComponent::Tags::Enemy && otherTag == TagComponent::Tags::Bullet))
		{
			TakeDamage(1);
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

bool dae::ColliderComponent::CheckCollision(const ColliderComponent& other)
{
	bool isOverlapping = IsOverlapping(other);

	if (isOverlapping)
	{
		Event e(make_sdbm_hash("OnCollision"));
		GameObject* otherOwner = other.GetOwner();
		m_subject.NotifyObservers(e, (void*)otherOwner);
	}

	return isOverlapping;
}

dae::ColliderComponent::Rect dae::ColliderComponent::GetWorldRect() const
{
	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
	return { pos.x, pos.y, m_width, m_height };
}

bool dae::ColliderComponent::IsOverlapping(const ColliderComponent& other) const
{
	auto a = GetWorldRect();
	auto b = other.GetWorldRect();

	return (a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.h &&
		a.y + a.h > b.y);
}

void dae::ShootComponent::Update(float delta_time)
{
	m_timer -= delta_time;
}

void dae::ShootComponent::Shoot()
{
	if (m_timer > 0.f) return;

	m_timer = m_cooldown;

	//sound
	auto& sound = dae::servicelocator::get_sound_system();
	sound.play(0, 0.5f);

	Event e(make_sdbm_hash("SpawnBullet"));
	m_subject.NotifyObservers(e, GetOwner());
}

void dae::VelocityComponent::Update(float delta_time)
{
	auto pos = GetOwner()->GetTransform().GetPosition();
	GetOwner()->SetPosition(
		pos.x + m_velocity.x * delta_time,
		pos.y + m_velocity.y * delta_time
	);
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
    std::string text = "Score: " + std::to_string(m_score);
    SetText(text);
}

#pragma endregion

