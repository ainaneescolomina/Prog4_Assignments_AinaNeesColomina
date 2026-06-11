#pragma once
#include "Observer.h"

namespace dae
{
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
    };
}