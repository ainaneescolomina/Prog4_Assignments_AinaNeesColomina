#pragma once
#include <memory>
#include "EnemyState.h"
#include <glm/glm.hpp>
#include "EnemyType.h"

namespace dae
{
    struct DiveData
    {
        float attackY;
        float orbitRadius;
        float rotationSpeed;
        int minLoops;
        int maxLoops;
        float horizontalOffset;
    };

    class GameObject;

    class EnemyDiveState final : public EnemyState
    {
    public:
        virtual ~EnemyDiveState() = default;

        virtual void OnEnter(GameObject* owner);
        virtual void OnExit(GameObject* owner);

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time);

    private:
        glm::vec2 m_startPos{};

        std::vector<glm::vec2> m_points{};
        int m_currentPoint{ 0 };

        float m_speed{ 300.f };
        float m_xOffset{ 0.f };
        float m_sway{ 0.f };

    };
}