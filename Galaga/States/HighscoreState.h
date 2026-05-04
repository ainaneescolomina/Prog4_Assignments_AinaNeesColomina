#include <memory>
#include "GameState.h"

class HighscoreState final : public dae::GameState
{
public:
    void OnEnter() override;

    std::unique_ptr<dae::GameState> Update(float deltaTime) override;

private:
};