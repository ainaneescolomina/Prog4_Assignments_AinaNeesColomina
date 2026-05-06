#pragma once
#include <memory>
#include "GameState.h"
#include "Singleton.h"

namespace dae 
{
    class GameStateManager : public Singleton<GameStateManager>
    {
    public:
        void Update(float delta_time);
        void ChangeState(std::unique_ptr<GameState> newState);

    private:
        std::unique_ptr<GameState> m_pCurrentState;
    };
}