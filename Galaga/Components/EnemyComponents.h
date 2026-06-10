#include "Components.h"
#include "EnemyState.h"
#include "EnemyType.h"

namespace dae
{
    class EnemyComponent final : public dae::Component {
    public:
        explicit EnemyComponent(dae::GameObject* owner, EnemyType type) : Component(owner), m_type(type) {}
        virtual ~EnemyComponent();
        EnemyComponent(const EnemyComponent& other) = delete;
        EnemyComponent(EnemyComponent&& other) = delete;
        EnemyComponent& operator=(const EnemyComponent& other) = delete;
        EnemyComponent& operator=(EnemyComponent&& other) = delete;

        EnemyType GetType() const { return m_type; }

        void SetDiving(bool diving) { m_isDiving = diving; }
        bool IsDiving() const { return m_isDiving; }

        int GetScoreValue() const;

    private:
        EnemyType m_type;
        bool m_isDiving{};
    };
}

class EnemyStateComponent final : public dae::Component {
public:
    explicit EnemyStateComponent(dae::GameObject* owner) : Component(owner) {}
    virtual ~EnemyStateComponent();
    EnemyStateComponent(const EnemyStateComponent& other) = delete;
    EnemyStateComponent(EnemyStateComponent&& other) = delete;
    EnemyStateComponent& operator=(const EnemyStateComponent& other) = delete;
    EnemyStateComponent& operator=(EnemyStateComponent&& other) = delete;

    void Update(float delta_time) override;

    void SetState(std::unique_ptr<dae::EnemyState> newState);

private:
    std::unique_ptr<dae::EnemyState> m_pCurrentState;
};