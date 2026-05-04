#include <memory>
#include "GameState.h"
#include "Commands.h"

namespace dae 
{
    class Scene;
    class MenuState final : public dae::GameState
    {
    public:
        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float) override;

    private:
        Scene* m_pScene = nullptr;
    };

    class StartGameCommand final : public Command {
    public:
        void Execute(float) override;
    };
}