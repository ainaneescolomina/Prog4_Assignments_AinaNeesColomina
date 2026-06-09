#pragma once
#include "Components.h"

namespace dae
{
	class GameObject;

	// --- GAME ACTOR ---
	class LivesComponent final : public Component, public Observer
	{
	public:
		enum class DeathAction
		{
			Destroy,
			Deactivate
		};

		explicit LivesComponent(GameObject* ownerRef, int lives, DeathAction action)
			: Component(ownerRef), m_lives{ lives }, m_deathAction{ action }, m_subject(this) {
		}
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
		DeathAction m_deathAction;
		Subject m_subject;
	};

	class ScoreComponent final : public Component, public Observer
	{
	public:
		explicit ScoreComponent(GameObject* ownerRef) : Component(ownerRef), m_subject(this) {}
		virtual ~ScoreComponent() = default;
		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		virtual void Notify(Event event, void* sender) override;

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
		enum class Tags
		{
			Player,
			Enemy,
			Bullet
		};

		explicit TagComponent(GameObject* ownerRef, Tags tag) : Component(ownerRef), m_tag{ tag } {}
		virtual ~TagComponent() = default;
		TagComponent(const TagComponent& other) = delete;
		TagComponent(TagComponent&& other) = delete;
		TagComponent& operator=(const TagComponent& other) = delete;
		TagComponent& operator=(TagComponent&& other) = delete;

		void ChangeTag(Tags tag) { m_tag = tag; };
		Tags GetTag() const { return m_tag; };

	private:
		Tags m_tag;
	};

	class ShootComponent final : public Component
	{
	public:
		explicit ShootComponent(GameObject* ownerRef, float cooldown) : Component(ownerRef), m_cooldown{ cooldown }
			, m_subject(this) {
		}
		virtual ~ShootComponent() = default;
		ShootComponent(const ShootComponent& other) = delete;
		ShootComponent(ShootComponent&& other) = delete;
		ShootComponent& operator=(const ShootComponent& other) = delete;
		ShootComponent& operator=(ShootComponent&& other) = delete;

		virtual void Update(float delta_time) override;

		void Shoot();

		Subject& GetSubject() { return m_subject; }

	private:
		float m_cooldown;
		float m_timer{};

		Subject m_subject;
	};

	class DamageManager final : public Component, public Observer
	{
	public:
		explicit DamageManager(GameObject* ownerRef) : Component(ownerRef), m_subject(this) {}
		virtual ~DamageManager() = default;
		DamageManager(const DamageManager& other) = delete;
		DamageManager(DamageManager&& other) = delete;
		DamageManager& operator=(const DamageManager& other) = delete;
		DamageManager& operator=(DamageManager&& other) = delete;

		virtual void Notify(Event event, void* sender) override;

		void AddThreat(TagComponent::Tags threat);

		Subject& GetSubject() { return m_subject; }

	private:
		std::vector<TagComponent::Tags> m_threats;
		Subject m_subject;
	};

	class OffscreenDeactivateComponent final : public Component
	{
	public:
		OffscreenDeactivateComponent(GameObject* owner,float height, float width)
			: Component(owner), m_height(height), m_width(width){}

		virtual void Update(float delta_time) override;

	private:
		float m_height;
		float m_width;
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

		int m_score{ 0 };
	};
}