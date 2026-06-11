#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;

    class ButterflyDiveState final : public EnemyState
    {
    public:
        virtual ~ButterflyDiveState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Dive; }

        virtual void OnEnter(GameObject* owner) override;
        virtual void OnExit(GameObject* owner) override;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;

    private:
        float m_speed{ 340.f };
        float m_segmentSpeed{ 0.f };

        std::vector<glm::vec2> m_points{};
        int m_currentPoint{ 0 };

        // shooting
        float m_timer{ };
        float m_shootingDelay{ 0.25f };
        int m_amountBullets{ 0 };
    };
}