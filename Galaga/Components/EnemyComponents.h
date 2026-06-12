#include "Components.h"
#include "EnemyState.h"
#include "EnemyType.h"
#include <glm/glm.hpp>

namespace dae
{
    class EnemyComponent final : public dae::Component, public Observer
    {
    public:
        explicit EnemyComponent(dae::GameObject* owner, EnemyType type) : Component(owner), m_type(type) {}
        virtual ~EnemyComponent();
        EnemyComponent(const EnemyComponent& other) = delete;
        EnemyComponent(EnemyComponent&& other) = delete;
        EnemyComponent& operator=(const EnemyComponent& other) = delete;
        EnemyComponent& operator=(EnemyComponent&& other) = delete;

        virtual void Notify(Event event, void* sender) override;
        void AddSubscription(dae::Subscription subscription)
        {
            m_subscriptions.emplace_back(std::move(subscription));
        }

        EnemyType GetType() const { return m_type; }

        void SetDiving(bool diving) { m_isDiving = diving; }
        bool IsDiving() const { return m_isDiving; }

        int GetScoreValue() const;

        void SetFormationPosition(const glm::vec2& pos) { m_formationPosition = pos; }
        const glm::vec2& GetFormationPosition() const { return m_formationPosition; }

        void SetBeamObject(GameObject* beam) { m_pBeamObject = beam; }
        GameObject* GetBeamObject() const { return m_pBeamObject; }

        dae::GameObject* GetEnemyOwner() const { return GetOwner(); }

    private:
        EnemyType m_type;
        bool m_isDiving{};
        glm::vec2 m_formationPosition{};

        // Boss Galaga Beam
        GameObject* m_pBeamObject{};

        std::vector<dae::Subscription> m_subscriptions;
    };
}

// PATTERN: State Pattern
// DESCRIPTION: Controller for the enemy behavior state machine
class EnemyStateComponent final : public dae::Component 
{
public:
    explicit EnemyStateComponent(dae::GameObject* owner) : Component(owner) {}
    virtual ~EnemyStateComponent();
    EnemyStateComponent(const EnemyStateComponent& other) = delete;
    EnemyStateComponent(EnemyStateComponent&& other) = delete;
    EnemyStateComponent& operator=(const EnemyStateComponent& other) = delete;
    EnemyStateComponent& operator=(EnemyStateComponent&& other) = delete;

    void Update(float delta_time) override;

    void SetState(std::unique_ptr<dae::EnemyState> newState);
    dae::EnemyState* GetCurrentState() const { return m_pCurrentState.get(); }

private:
    std::unique_ptr<dae::EnemyState> m_pCurrentState;
};