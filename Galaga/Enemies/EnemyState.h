#pragma once
#include <memory>

namespace dae 
{
    class GameObject;

    enum class EnemyStateType
    {
        Formation,
        Idle,
        Dive
    };

    class EnemyState 
    {
    public:
        virtual ~EnemyState() = default;

        virtual EnemyStateType GetType() const = 0;

        virtual void OnEnter(GameObject* owner) = 0;
        virtual void OnExit(GameObject* owner) = 0;

        virtual std::unique_ptr<EnemyState> Update(GameObject* owner, float delta_time) = 0;
    };
}