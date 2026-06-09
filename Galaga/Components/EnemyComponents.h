#include "Components.h"
#include "EnemyState.h"
#include "EnemyType.h"

namespace dae
{
    class EnemyComponent final : public dae::Component {
    public:
        explicit EnemyComponent(dae::GameObject* owner, EnemyType type) : Component(owner), m_Type(type) {}
        virtual ~EnemyComponent();
        EnemyComponent(const EnemyComponent& other) = delete;
        EnemyComponent(EnemyComponent&& other) = delete;
        EnemyComponent& operator=(const EnemyComponent& other) = delete;
        EnemyComponent& operator=(EnemyComponent&& other) = delete;

        EnemyType GetType() const { return m_Type; }

        void SetDiving(bool diving) { m_IsDiving = diving; }
        bool IsDiving() const { return m_IsDiving; }

        int GetScoreValue() const;

    private:
        EnemyType m_Type;
        bool m_IsDiving{};
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