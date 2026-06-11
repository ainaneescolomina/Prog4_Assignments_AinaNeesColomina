#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
    class GameObject;
    class EnemyComponent;

    class GalagaTractorBeamState final : public EnemyState
    {
    public:
        virtual ~GalagaTractorBeamState() = default;

        EnemyStateType GetType() const override { return EnemyStateType::Dive; }

        virtual void OnEnter(GameObject* owner) override;
        virtual void OnExit(GameObject* owner) override;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) override;

    private:
        dae::EnemyComponent* m_enemyComp{};

        glm::vec2 m_startPos{};
        float m_speed{ 220.f };
        float m_segmentSpeed{ 0.f };

        std::vector<glm::vec2> m_points{};
        int m_currentPoint{ 0 };

        bool m_beamActive{ false };
        float m_beamTimer{ 0.f };
        float m_beamDuration{ 3.f };
    };
}