#include "Components.h"

#include <iostream>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

//dae::TextComponent::TextComponent(GameObject* ownerRef, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color)
//	: RenderComponent(ownerRef), m_needsUpdate(true), m_text(text), m_color(color), m_font(std::move(font)), m_textTexture(nullptr)
//{ }

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
		const auto& pos = m_transform.GetPosition();
		Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetPosition(const float x, const float y)
{
	m_transform.SetPosition(x, y);
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_color = color;
	m_needsUpdate = true;
}

void dae::FPSComponent::Update(float delta_time)
{
	(void)delta_time;

	m_frameCount++;
	m_elapsedTime += delta_time;
	if (m_elapsedTime >= 1.0f)
	{
		m_fps = m_frameCount / m_elapsedTime;
		m_frameCount = 0;
		m_elapsedTime = 0.0f;

		std::cout << "FPS: " << m_fps << std::endl;
	}
}

float dae::FPSComponent::GetFPS() const
{
	return m_fps;
}


