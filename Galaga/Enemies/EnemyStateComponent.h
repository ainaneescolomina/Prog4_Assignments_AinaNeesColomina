#include "Components.h"
#include "EnemyState.h"

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