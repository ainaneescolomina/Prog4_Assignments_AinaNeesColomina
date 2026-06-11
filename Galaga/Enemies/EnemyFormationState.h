#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;

    class EnemyFormationState final : public EnemyState
    {
    public:
        explicit EnemyFormationState(float speed) : m_speed{ speed } {}
        virtual ~EnemyFormationState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Formation; }

        virtual void OnEnter(GameObject* owner) override;
        virtual void OnExit(GameObject* owner) override;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;
    private:
        float m_speed;
        glm::vec2 m_targetPos{};
    };
}