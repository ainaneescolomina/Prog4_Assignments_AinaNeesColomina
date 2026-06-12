#pragma once
#include "GameState.h"
#include "Commands.h"
#include <string>
#include <memory>

// REFERENCE: https://forum.gamemaker.io/index.php?threads/solved-arcade-style-highscore-table.45963/

namespace dae {
    class Scene;
    class GameObject;
    class TextComponent;

    class NameEntryState final : public GameState
    {
    public:
        explicit NameEntryState(int finalScore) : m_finalScore{ finalScore } {}
        virtual ~NameEntryState() = default;

        void OnEnter() override;
        void OnExit() override;
        std::unique_ptr<GameState> Update(float delta_time) override;

        void CycleLetter(int change);
        void ConfirmLetter();

    private:
        void UpdateNameEntryText();

        Scene* m_pScene = nullptr;
        int m_finalScore;

        std::string m_name{ "AAA" };
        int m_editIdx{ 0 };

        TextComponent* m_pNameEntryText = nullptr;
    };

    class CycleLetterCommand final : public dae::Command 
    {
    public:
        explicit CycleLetterCommand(NameEntryState* state, int direction)
            : m_pState{ state }, m_direction{ direction } {}

        void Execute(float, float) override;

    private:
        NameEntryState* m_pState;
        int m_direction;
    };

    class ConfirmLetterCommand final : public dae::Command 
    {
    public:
        explicit ConfirmLetterCommand(NameEntryState* state) : m_pState{ state } {}

        void Execute(float, float) override;

    private:
        NameEntryState* m_pState;
    };
}