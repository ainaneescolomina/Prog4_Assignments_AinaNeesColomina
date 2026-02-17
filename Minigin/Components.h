#pragma once

#include "Transform.h"
#include <string>
#include <memory>
#include <SDL3/SDL.h>

namespace dae
{
	class GameObject;

	class Component
	{
	public:
		Component(GameObject* ownerRef) : m_owner{ ownerRef } {};
		virtual ~Component() = default;

		virtual void Update(float) {}
		virtual void Render() const {}

	protected:
		GameObject* m_owner;
	};

	class Texture2D;

	class RenderComponent : public Component
	{
	public:
		explicit RenderComponent(GameObject* ownerRef) : Component(ownerRef) {};
		virtual ~RenderComponent() = default;

		virtual void Render() const override;

		void SetTexture(const std::string& filename);

	private:
		std::shared_ptr<Texture2D> m_texture{};
	};

	class Font;

	class TextComponent : public Component
	{
	public:
		void Update(float delta_time) override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

		explicit TextComponent(GameObject* ownerRef, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color = { 255, 255, 255, 255 })
			: Component(ownerRef), m_needsUpdate(true),
			m_text(text), m_color(color), m_font(std::move(font)),
			m_textTexture(nullptr)
		{}
		virtual ~TextComponent() = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

	private:
		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};
		std::shared_ptr<Texture2D> m_textTexture{};
	};

	class FPSComponent : public Component
	{
	public:
		explicit FPSComponent(GameObject* ownerRef) : Component(ownerRef) {};
		virtual ~FPSComponent() = default;

		virtual void Update(float delta_time) override;

		float GetFPS() const;

	private:
		int m_frameCount = 0;
		float m_fps = 0.0f;
		float m_elapsedTime = 0.0f;
	};
}