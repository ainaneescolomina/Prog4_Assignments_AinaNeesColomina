#include "Components.h"
#include "Components.h"
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


#pragma region --- BASE ---

void dae::RenderComponent::Render() const
{
	if (m_texture == nullptr) return;
	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
	glm::vec2 texturePos{ pos.x, pos.y };

	if (m_centerTexture)
	{
		auto textureSize = m_texture->GetSize();
		texturePos.x = pos.x - (textureSize.x / 2);
		texturePos.y = pos.y - (textureSize.y / 2);
	}

	Renderer::GetInstance().RenderTexture(*m_texture, texturePos.x, texturePos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename, bool center)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
	m_centerTexture = center;
}

dae::AnimatedRenderComponent::AnimatedRenderComponent(GameObject* ownerRef, int rows, int cols, float frameTime)
	: RenderComponent(ownerRef)
	, m_rows(rows), m_cols(cols)
	, m_frameTime(frameTime), m_totalFrames(rows* cols)
{
}

void dae::AnimatedRenderComponent::Update(float delta_time)
{
	if (m_totalFrames <= 1) return;

	m_timer += delta_time;

	if (m_timer >= m_frameTime)
	{
		m_timer -= m_frameTime;
		m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
	}
}

void dae::AnimatedRenderComponent::Render() const
{
	if (m_texture == nullptr) return;

	if (m_totalFrames <= 1)
	{
		dae::RenderComponent::Render();
		return;
	}

	int col = m_currentFrame % m_cols;
	int row = m_currentFrame / m_cols;

	float srcX = static_cast<float>(col * m_frameWidth);
	float srcY = static_cast<float>(row * m_frameHeight);

	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
	glm::vec2 texturePos{ pos.x, pos.y };

	if (m_centerTexture)
	{
		texturePos.x = pos.x - (m_frameWidth / 2.f);
		texturePos.y = pos.y - (m_frameHeight / 2.f);
	}

	dae::Renderer::GetInstance().RenderTexture(
		*m_texture,
		texturePos.x, texturePos.y,
		static_cast<float>(m_frameWidth), static_cast<float>(m_frameHeight),
		srcX, srcY,
		static_cast<float>(m_frameWidth), static_cast<float>(m_frameHeight)
	);
}

void dae::AnimatedRenderComponent::SetTexture(const std::string& filename, int rows, int cols, float frameTime, bool center)
{
	dae::RenderComponent::SetTexture(filename, center);

	if (m_texture)
	{
		m_rows = rows;
		m_cols = cols;
		m_totalFrames = rows * cols;

		m_frameTime = frameTime;
		m_currentFrame = 0;
		m_timer = 0.0f;

		auto textureSize = m_texture->GetSize();
		m_frameWidth = static_cast<int>(textureSize.x / m_cols);
		m_frameHeight = static_cast<int>(textureSize.y / m_rows);
	}
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

void dae::RotatorComponent::Update(float delta_time)
{
    m_angle += m_speed * delta_time;
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

void dae::ColliderComponent::Render() const
{
	if (!m_debugg) return;
	auto rect = GetWorldRect();

	SDL_FRect sdlRect{ rect.x, rect.y, rect.w, rect.h };
	auto* sdlRenderer = Renderer::GetInstance().GetSDLRenderer();
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(sdlRenderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);
	SDL_RenderRect(sdlRenderer, &sdlRect);
	SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a);
}

bool dae::ColliderComponent::CheckCollision(const ColliderComponent& other)
{
	bool isOverlapping = IsOverlapping(other);

	if (isOverlapping)
	{
		Event e(make_sdbm_hash("OnCollision"));
		GameObject* otherOwner = other.GetOwner();
		if(otherOwner) m_subject.NotifyObservers(e, (void*)otherOwner);
	}

	return isOverlapping;
}

dae::ColliderComponent::Rect dae::ColliderComponent::GetWorldRect() const
{
	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();

	// center to point
	float centerX = pos.x - (m_width / 2.f);
	float centerY = pos.y - (m_height / 2.f);

	return { centerX, centerY, m_width, m_height };
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

void dae::VelocityComponent::Update(float delta_time)
{
	auto pos = GetOwner()->GetTransform().GetPosition();
	GetOwner()->SetPosition(
		pos.x + m_velocity.x * delta_time,
		pos.y + m_velocity.y * delta_time
	);
}

void dae::VelocityComponent::SetVelocity(float vx, float vy)
{
	m_velocity = { vx, vy };
}

#pragma endregion


