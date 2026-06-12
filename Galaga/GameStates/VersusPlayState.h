#pragma once
#include <memory>
#include "GameState.h"
#include "BulletSpawner.h"
#include "LevelManager.h"
#include "GameStatsManager.h"

namespace dae
{
    class Scene;
    class VersusPlayState final : public dae::GameState, public Observer
    {
    public:
        // GameState Functions
        void OnEnter() override;
        void OnExit() override;

        std::unique_ptr<dae::GameState> Update(float delta_time) override;

        // Observer Functions
        virtual void Notify(Event event, void* sender) override;
        void AddSubscription(dae::Subscription subscription)
        {
            m_subscriptions.emplace_back(std::move(subscription));
        }

    private:
        Scene* m_pScene = nullptr;
        std::vector<dae::Subscription> m_subscriptions;

        bool m_playerDied{};

        std::unique_ptr<dae::BulletSpawner> m_pBulletSpawner;
        std::unique_ptr<LevelManager> m_pLevelManager;
        std::unique_ptr<GameStatsManager> m_pGameStats;
    };
}