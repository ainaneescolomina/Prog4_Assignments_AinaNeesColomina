#pragma once
#include "Observer.h"
#include <vector>

namespace dae
{
    // PATTERN: Observer Pattern
    // DESCRIPTION: Tracks gameplay metrics (shots, hits, final score)
    class GameStatsManager final : public Observer
    {
    public:
        GameStatsManager() = default;
        ~GameStatsManager() = default;

        GameStatsManager(const GameStatsManager& other) = delete;
        GameStatsManager(GameStatsManager&& other) = delete;
        GameStatsManager& operator=(const GameStatsManager& other) = delete;
        GameStatsManager& operator=(GameStatsManager&& other) = delete;

        void Notify(Event event, void* sender) override;
        void AddSubscription(Subscription subscription);

        void AddShot() { ++m_shotsFired; }
        void AddHit() { ++m_hitsScored; }
        void SetScore(int score) { m_score = score; }

        // Getters
        int GetShotsFired() const { return m_shotsFired; }
        int GetHitsScored() const { return m_hitsScored; }
        int GetFinalScore() const { return m_score; }

        int GetHitMissRatio() const;

    private:
        int m_shotsFired{ 0 };
        int m_hitsScored{ 0 };
        int m_score{ 0 };

        std::vector<Subscription> m_subscriptions;
    };
}