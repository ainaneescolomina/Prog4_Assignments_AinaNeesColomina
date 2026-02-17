#include "Components.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Renderer.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h"

void dae::RenderComponent::Render() const
{
	if (m_texture == nullptr) return;
	const auto& pos = m_owner->GetTransform().GetPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextComponent::Update(float delta_time)
{
	(void)delta_time;

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
		m_textTexture = std::make_shared<Texture2D>(texture);
		m_needsUpdate = false;
	}
}

void dae::TextComponent::Render() const
{
	if (m_textTexture != nullptr)
	{
		const auto& pos = m_owner->GetTransform().GetPosition();
		Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
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

void dae::FPSComponent::Update(float delta_time)
{
	m_elapsedTime += delta_time;
	m_frameCount++;

	if (m_elapsedTime >= 1.0f)
	{
		m_fps = static_cast<float>(m_frameCount) / m_elapsedTime;

		auto pTextComponent = m_owner->GetComponent<TextComponent>();
		if (pTextComponent)
		{
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(2) << m_fps;
			pTextComponent->SetText(ss.str() + " FPS");
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
