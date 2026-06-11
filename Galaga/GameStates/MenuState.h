#pragma once
#include <memory>
#include <vector>
#include "GameState.h"
#include "Commands.h"

namespace dae 
{
    class Scene;
    class TextComponent;

    class MenuState final : public dae::GameState
    {
    public:
        enum class GameMode
        {
            SinglePlayer,
            CoOp,
            Versus,
            Highscore,
            Quit
        };

        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float) override;

        // menu navigation
        void MoveSelection(int direction);
        void ConfirmSelection();

    private:
        void UpdateModeSelection();

        Scene* m_pScene = nullptr;
        int m_selectedIdx{ 0 };
        std::vector<TextComponent*> m_pMenuTexts{};
    };

    class NavigateMenuCommand final : public Command 
    {
    public:
        explicit NavigateMenuCommand(MenuState* menu, int direction) : m_pMenu{ menu }, m_direction{ direction } {}
        void Execute(float) override { m_pMenu->MoveSelection(m_direction); }
    
    private:
        MenuState* m_pMenu;
        int m_direction;
    };

    class ConfirmSelectionCommand final : public Command 
    {
    public:
        explicit ConfirmSelectionCommand(MenuState* menu) : m_pMenu{ menu } {}
        void Execute(float) override { m_pMenu->ConfirmSelection(); }
    
    private:
        MenuState* m_pMenu;
    };
}