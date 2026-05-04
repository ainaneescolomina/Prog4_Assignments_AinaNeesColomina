#pragma once
#include <memory>
#include "GameState.h"

namespace dae 
{

    class GameStateManager 
    {
    public:
        void Update(float deltaTime);
        void ChangeState(std::unique_ptr<GameState> newState);

    private:
        std::unique_ptr<GameState> m_pCurrentState;
    };

}