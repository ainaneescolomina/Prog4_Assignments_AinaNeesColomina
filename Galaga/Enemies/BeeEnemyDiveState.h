#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;

    class BeeEnemyDiveState : public EnemyState
    {
    public:
        virtual ~BeeEnemyDiveState() = default;

        virtual void OnEnter(GameObject* owner);
        virtual void OnExit(GameObject* owner);

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time);

    private:
        float m_speed = 150.f;
        glm::vec2 m_startPos;
    };
}