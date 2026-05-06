#pragma once
#include <memory>
#include "EnemyState.h"

namespace dae
{
    class GameObject;

    class EnemyIdleState : public EnemyState
    {
    public:
        virtual ~EnemyIdleState() = default;

        virtual void OnEnter(GameObject* owner);
        virtual void OnExit(GameObject* owner);

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time);

    private:
        float m_timer = 0.f;
        float m_waitAttack;
    };
}