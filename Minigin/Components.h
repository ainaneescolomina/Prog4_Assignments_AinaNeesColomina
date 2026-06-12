#pragma once

#include "Transform.h"
#include "Subject.h"
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

	// --- BASE ---
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

		virtual void SetTexture(const std::string& filename, bool center = false);

	protected:
		std::shared_ptr<Texture2D> m_texture{};
		bool m_centerTexture{ false };
	};

	class AnimatedRenderComponent final : public RenderComponent
	{
	public:
		AnimatedRenderComponent(GameObject* ownerRef, int rows, int cols, float frameTime);
		virtual ~AnimatedRenderComponent() = default;
		AnimatedRenderComponent(const AnimatedRenderComponent& other) = delete;
		AnimatedRenderComponent(AnimatedRenderComponent&& other) = delete;
		AnimatedRenderComponent& operator=(const AnimatedRenderComponent& other) = delete;
		AnimatedRenderComponent& operator=(AnimatedRenderComponent&& other) = delete;

		virtual void Update(float delta_time) override;
		virtual void Render() const override;

		virtual void SetTexture(const std::string& filename, bool center = false) override { SetTexture(filename, m_rows, m_cols, m_frameTime, center); }
		void SetTexture(const std::string& filename, int rows, int cols, float frameTime, bool center = false);

	private:
		int m_rows{ 1 };
		int m_cols{ 1 };

		int m_frameWidth{ 0 };
		int m_frameHeight{ 0 };

		float m_frameTime{ 0.1f };
		float m_timer{ 0.0f };

		int m_currentFrame{ 0 };
		int m_totalFrames{ 0 };
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
		explicit RotatorComponent(GameObject* ownerRef, float radius, float speed);
		virtual ~RotatorComponent() = default;
		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		virtual void Update(float delta_time) override;

	private:
		glm::vec3 m_startPosition{};
		float m_radius{};
		float m_speed{};
		float m_angle{};
	};

	// --- GAME ACTOR ---

	class ColliderComponent final : public Component
	{
	public:
		explicit ColliderComponent(GameObject* ownerRef, float width, float height) 
			: Component(ownerRef)
			, m_width(width), m_height(height)
			, m_subject(this){}
		virtual ~ColliderComponent() = default;
		ColliderComponent(const ColliderComponent& other) = delete;
		ColliderComponent(ColliderComponent&& other) = delete;
		ColliderComponent& operator=(const ColliderComponent& other) = delete;
		ColliderComponent& operator=(ColliderComponent&& other) = delete;

		virtual void Render() const override;

		struct Rect
		{
			float x, y, w, h;
		};

		bool CheckCollision(const ColliderComponent& other);

		bool IsActive() const { return m_active; }
		void SetActive(bool active) { m_active = active; }

		Subject& GetSubject() { return m_subject; };

	private:
		float m_width{};
		float m_height{};
		Subject m_subject;
		bool m_active{ true };
		bool m_debugg{ false };

		Rect GetWorldRect() const;
		bool IsOverlapping(const ColliderComponent& other) const;
	};

	class VelocityComponent  final : public Component
	{
	public:
		explicit VelocityComponent(GameObject* owner, float vx, float vy)
			: Component(owner), m_velocity{ vx, vy } {
		}

		virtual ~VelocityComponent() = default;
		VelocityComponent(const VelocityComponent& other) = delete;
		VelocityComponent(VelocityComponent&& other) = delete;
		VelocityComponent& operator=(const VelocityComponent& other) = delete;
		VelocityComponent& operator=(VelocityComponent&& other) = delete;

		virtual void Update(float delta_time) override;

		void SetVelocity(float vx, float vy);

	private:
		glm::vec2 m_velocity;
	};
}