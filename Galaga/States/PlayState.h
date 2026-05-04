#include <memory>
#include "GameState.h"
#include "Achievements.h"
#include "../BulletSpawner.h"
#include "../WaveSpawner.h"

class PlayState final : public dae::GameState 
{
public:
    void OnEnter() override;

    std::unique_ptr<dae::GameState> Update(float deltaTime) override;

private:
    bool m_playerDied;

    dae::WinOneGameAchievement m_pWinAchievement;
    std::unique_ptr<dae::BulletSpawner> m_pBulletSpawner;
    std::unique_ptr<WaveSpawner> m_pWaveSpawner;
};