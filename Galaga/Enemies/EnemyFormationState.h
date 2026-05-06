#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;

    class EnemyFormationState : public EnemyState
    {
    public:
        explicit EnemyFormationState(float speed, const glm::vec2& target) : m_speed{ speed }, m_targetPos{ target } {}
        virtual ~EnemyFormationState() = default;

        virtual void OnEnter(GameObject* owner);
        virtual void OnExit(GameObject* owner);

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time);
    private:
        float m_speed;
        glm::vec2 m_targetPos;
    };
}