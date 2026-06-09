#pragma once
#include <memory>
#include "GameState.h"
#include "Commands.h"
//#include "Achievements.h"
#include "BulletSpawner.h"
#include "WaveSpawner.h"

namespace dae
{
    class Scene;
    class PlayState final : public dae::GameState, public Observer
    {
    public:
        // GameState Functions
        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float delta_time) override;

        // Observer Functions
        virtual void Notify(Event event, void* sender) override;

        // Class Functions
        void SkipLevel();

    private:
        Scene* m_pScene = nullptr;

        bool m_playerDied{};

        //dae::WinOneGameAchievement m_pWinAchievement;
        std::unique_ptr<dae::BulletSpawner> m_pBulletSpawner;
        std::unique_ptr<WaveSpawner> m_pWaveSpawner;
    };

    class SkipLevelCommand final : public Command {
    public:
        SkipLevelCommand(PlayState* obj) : m_pPlayState(obj) {}
        void Execute(float) override;

    private:
        PlayState* m_pPlayState;
    };
}