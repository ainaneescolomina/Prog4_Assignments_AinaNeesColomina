#pragma once
#include "EnemyState.h"

namespace dae
{
    class EnemyDeadState final : public EnemyState
    {
    public:
        EnemyDeadState() = default;
        virtual ~EnemyDeadState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Dead; }

        void OnEnter(GameObject* owner) override;
        void OnExit(GameObject*) override {}
        std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;

    private:
        float m_deathTimer{ 1.0f }; // Wait exactly 1.0 second
    };
}