#pragma once
#include <memory>
#include "EnemyState.h"

namespace dae
{
    class GameObject;

    class EnemyIdleState final : public EnemyState
    {
    public:
        virtual ~EnemyIdleState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Idle; }

        virtual void OnEnter(GameObject* owner) override;
        virtual void OnExit(GameObject* owner) override;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;

        void SetFormationOffset(float x) { m_formationOffset = x; }

    private:
        float m_formationOffset;
    };
}