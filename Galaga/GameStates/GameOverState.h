#include <memory>
#include "GameState.h"
#include "Commands.h"

namespace dae {
    class Scene;

    class GameOverState final : public GameState
    {
    public:
        explicit GameOverState(int shotsFired, int numberHits, int ratio, int score)
            : m_shotsFired{ shotsFired }, m_numberHits{ numberHits }, m_ratio{ ratio }, m_score{ score } {}

        void OnEnter() override;
        void OnExit() override;
        std::unique_ptr<GameState> Update(float delta_time) override;

    private:
        Scene* m_pScene = nullptr;

        int m_shotsFired;
        int m_numberHits;
        int m_ratio;
        int m_score;
    };

    class OpenHighscoreMenuCommand final : public dae::Command {
    public:
        explicit OpenHighscoreMenuCommand(int score) : m_score{ score } {}
        void Execute(float, float) override;

    private:
        int m_score;
    };
}