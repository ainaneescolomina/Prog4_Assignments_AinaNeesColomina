#pragma once
#include <memory>
#include "GameState.h"
//#include "Achievements.h"
#include "BulletSpawner.h"
#include "LevelManager.h"
#include "GameStatsManager.h"

namespace dae
{
    class Scene;
    class SinglePlayerState final : public dae::GameState, public Observer
    {
    public:
        // GameState Functions
        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float delta_time) override;

        // Observer Functions
        virtual void Notify(Event event, void* sender) override;

    private:
        Scene* m_pScene = nullptr;

        bool m_playerDied{};

        //dae::WinOneGameAchievement m_pWinAchievement;
        std::unique_ptr<dae::BulletSpawner> m_pBulletSpawner;
        std::unique_ptr<LevelManager> m_pLevelManager;
        std::unique_ptr<GameStatsManager> m_pGameStats;
    };
}