#include <memory>
#include "GameStateManager.h"
#include "GameState.h"
#include "Commands.h"

namespace dae 
{
    class MenuState final : public dae::GameState
    {
    public:
        MenuState(GameStateManager* pManager) : m_pManager(pManager) {}

        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float) override;

    private:
        GameStateManager* m_pManager;
    };

    class StartGameCommand final : public Command {
    public:
        StartGameCommand(GameStateManager* pManager) : m_pManager(pManager) {}

        void Execute(float) override;
    private:
        GameStateManager* m_pManager;
    };
}