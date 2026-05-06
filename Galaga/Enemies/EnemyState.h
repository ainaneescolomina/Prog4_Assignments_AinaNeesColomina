#pragma once
#include <memory>

namespace dae 
{
    class GameObject;

    class EnemyState 
    {
    public:
        virtual ~EnemyState() = default;

        virtual void OnEnter(GameObject* owner) = 0;
        virtual void OnExit(GameObject* owner) = 0;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) = 0;
    };
}