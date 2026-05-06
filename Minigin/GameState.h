#pragma once
#include <memory>

namespace dae 
{
    class GameState 
    {
    public:
        virtual ~GameState() = default;

        virtual void OnEnter() {}
        virtual void OnExit() {}

        virtual std::unique_ptr<GameState> Update(float delta_time) = 0;
    };
}