#include <memory>
#include "GameState.h"
#include "Commands.h"

namespace dae {
    class Scene;

    class HighscoreState final : public GameState
    {
    public:
        void OnEnter() override;
        void OnExit() override;
        std::unique_ptr<GameState> Update(float deltaTime) override;
    private:
        Scene* m_pScene = nullptr;
    };

    class ReturnGameCommand final : public dae::Command {
    public:
        void Execute(float) override;
    };
}