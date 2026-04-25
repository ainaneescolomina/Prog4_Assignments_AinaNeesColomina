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

		// Is it ok to be public?
	//protected:
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
		explicit RotatorComponent(GameObject* ownerRef, float radius, float speed);
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

	// --- GAME ACTOR ---
	class LivesComponent final : public Component, public Observer
	{
	public:
		explicit LivesComponent(GameObject* ownerRef, int lives) : Component(ownerRef), m_lives{lives}, m_subject(this) {}
		virtual ~LivesComponent() = default;
		LivesComponent(const LivesComponent& other) = delete;
		LivesComponent(LivesComponent&& other) = delete;
		LivesComponent& operator=(const LivesComponent& other) = delete;
		LivesComponent& operator=(LivesComponent&& other) = delete;

		virtual void Notify(Event event, void* sender) override;

		void TakeDamage(int damage);
		int GetLives() const { return m_lives; };

		Subject& GetSubject() { return m_subject; };

	private:
		int m_lives;
		Subject m_subject;
	};

	class ScoreComponent final : public Component
	{
	public:
		explicit ScoreComponent(GameObject* ownerRef) : Component(ownerRef), m_subject(this) {}
		virtual ~ScoreComponent() = default;
		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		void AddScore(int score);
		int GetScore() const { return m_score; };

		Subject& GetSubject() { return m_subject; };

	private:
		int m_score{};
		Subject m_subject;
	};

	class TagComponent final : public Component
	{
	public:
		explicit TagComponent(GameObject* ownerRef) : Component(ownerRef) {}
		virtual ~TagComponent() = default;
		TagComponent(const TagComponent& other) = delete;
		TagComponent(TagComponent&& other) = delete;
		TagComponent& operator=(const TagComponent& other) = delete;
		TagComponent& operator=(TagComponent&& other) = delete;

		void ChangeTag(std::string tag) { m_tag = tag; };
		std::string GetTag() const { return m_tag; };

	private:
		std::string m_tag{};
	};

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

		struct Rect
		{
			float x, y, w, h;
		};

		bool CheckCollision(const ColliderComponent& other);

		Subject& GetSubject() { return m_subject; };

	private:
		float m_width{};
		float m_height{};
		Subject m_subject;

		Rect GetWorldRect() const;
		bool IsOverlapping(const ColliderComponent& other) const;
	};

	// --- UI ---
	class LivesDisplayComponent : public RenderComponent, public Observer
	{
	public:
		explicit LivesDisplayComponent(GameObject* ownerRef, int lives) : RenderComponent(ownerRef), m_lives{ lives } {}
		virtual ~LivesDisplayComponent() = default;
		LivesDisplayComponent(const LivesDisplayComponent& other) = delete;
		LivesDisplayComponent(LivesDisplayComponent&& other) = delete;
		LivesDisplayComponent& operator=(const LivesDisplayComponent& other) = delete;
		LivesDisplayComponent& operator=(LivesDisplayComponent&& other) = delete;

		virtual void Notify(Event event, void* sender) override;

		virtual void Render() const override;

	private:
		int m_lives;
	};

	class ScoreDisplayComponent : public TextComponent, public Observer
	{
	public:
		explicit ScoreDisplayComponent(GameObject* ownerRef, std::shared_ptr<Font> font, const SDL_Color& color = { 255, 255, 255, 255 });
		virtual ~ScoreDisplayComponent() = default;
		ScoreDisplayComponent(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent(ScoreDisplayComponent&& other) = delete;
		ScoreDisplayComponent& operator=(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent& operator=(ScoreDisplayComponent&& other) = delete;

		virtual void Notify(Event event, void* sender) override;

	private:
		void UpdateText();

		int m_score{0};
	};
}