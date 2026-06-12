#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;

    class EnemyEntranceState final : public EnemyState
    {
    public:
        explicit EnemyEntranceState(float speed, const std::vector<glm::vec2>& path) 
            : m_speed{ speed }, m_entrancePath{ path }  {}
        virtual ~EnemyEntranceState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Formation; }

        virtual void OnEnter(GameObject* owner) override;
        virtual void OnExit(GameObject* owner) override;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;
    private:
        float m_speed;
        int m_currentPoint{ 0 };
        glm::vec2 m_currentDirection{};
        std::vector<glm::vec2> m_entrancePath;
    };
}