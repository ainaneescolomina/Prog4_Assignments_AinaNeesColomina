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
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		virtual void Update(float) {}
		virtual void Render() const {}

	protected:
		GameObject* GetOwner() const { return m_owner; };

	private:
		GameObject* m_owner;
	};

	class Texture2D;

	class RenderComponent : public Component
	{
	public:
		explicit RenderComponent(GameObject* ownerRef) : Component(ownerRef) {};
		virtual ~RenderComponent() = default;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		virtual void Render() const override;

		void SetTexture(const std::string& filename);

	protected:
		std::shared_ptr<Texture2D> m_texture{};
	};

	class Font;

	class TextComponent : public RenderComponent
	{
	public:
		virtual void Update(float delta_time) override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

		explicit TextComponent(GameObject* ownerRef, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color = { 255, 255, 255, 255 })
			: RenderComponent(ownerRef), m_needsUpdate(true),
			m_text(text), m_color(color), m_font(std::move(font))
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
	};

	class FPSComponent : public Component
	{
	public:
		explicit FPSComponent(GameObject* ownerRef);
		virtual ~FPSComponent() = default;
		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;

		virtual void Update(float delta_time) override;

		float GetFPS() const;

	private:
		TextComponent* m_pTextComponent{};
		int m_frameCount = 0;
		float m_fps = 0.0f;
		float m_elapsedTime = 0.0f;
	};

	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent(GameObject* ownerRef, float radius, float speed);
		virtual ~RotatorComponent() = default;
		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		virtual void Update(float deltaTime) override;

	private:
		glm::vec3 m_startPosition{};
		float m_radius{};
		float m_speed{};
		float m_angle{};
	};

	class Exercise1Component final : public Component
	{
	public:
		explicit Exercise1Component(GameObject* ownerRef) : Component(ownerRef) {};
		virtual ~Exercise1Component() = default;

		Exercise1Component(const Exercise1Component&) = delete;
		Exercise1Component(Exercise1Component&&) = delete;
		Exercise1Component& operator=(const Exercise1Component&) = delete;
		Exercise1Component& operator=(Exercise1Component&&) = delete;

		void Render() const override;

	private:
		mutable int m_value{ 100 };
		mutable std::vector<float> m_data;
		mutable bool m_hasResults{ false };
	};

	class Exercise2Component final : public Component
	{
	public:
		explicit Exercise2Component(GameObject* ownerRef) : Component(ownerRef) {};
		virtual ~Exercise2Component() = default;

		Exercise2Component(const Exercise2Component&) = delete;
		Exercise2Component(Exercise2Component&&) = delete;
		Exercise2Component& operator=(const Exercise2Component&) = delete;
		Exercise2Component& operator=(Exercise2Component&&) = delete;

		void Render() const override;

	private:
		mutable int m_value{ 100 };

		mutable std::vector<float> m_valueData;
		mutable std::vector<float> m_pointerData;

		mutable bool m_hasValueResults{ false };
		mutable bool m_hasPointerResults{ false };
	};
}