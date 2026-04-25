#include <memory>
#include <string>

#include "GameObject.h"

#include "InputManager.h"
#include "Commands.h"

#include <SDL3/SDL_keycode.h>

namespace ActorFactory
{
    // --- PLAYER ---
    std::unique_ptr<dae::GameObject> CreatePlayer(
        dae::InputManager& input,
        const glm::vec2& pos)
    {
        auto player = std::make_unique<dae::GameObject>();

        player->AddComponent<dae::RenderComponent>()
            ->SetTexture("player.png");

        player->SetPosition(pos.x, pos.y);

        player->AddComponent<dae::TagComponent>("Player");

        auto* collider = player->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* lives = player->AddComponent<dae::LivesComponent>(3);
        player->AddComponent<dae::ScoreComponent>();

        // Input bindings
        float speed = 25.f;

        input.BindCommand(SDLK_W, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(player.get(), 0.f, -speed));

        input.BindCommand(SDLK_S, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(player.get(), 0.f, speed));

        input.BindCommand(SDLK_A, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));

        input.BindCommand(SDLK_D, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));

        input.BindCommand(SDLK_C, dae::KeyState::Up,
            std::make_unique<dae::DamageCommand>(player.get()));

        input.BindCommand(SDLK_Z, dae::KeyState::Up,
            std::make_unique<dae::ScoreCommand>(player.get(), 10));

        input.BindCommand(SDLK_X, dae::KeyState::Up,
            std::make_unique<dae::ScoreCommand>(player.get(), 100));

        // Observer / Subject
        collider->GetSubject().AddObserver(lives);

        return player;
    }

    // --- ENEMY ---
    enum class EnemyType
    {
        Bee,
        Butterfly,
        Boss
    };

    std::unique_ptr<dae::GameObject> CreateEnemy(const glm::vec2& pos)
    {
        auto enemy = std::make_unique<dae::GameObject>();

        enemy->AddComponent<dae::RenderComponent>()
            ->SetTexture("enemy_butterfly.png");

        enemy->SetPosition(pos.x, pos.y);

        enemy->AddComponent<dae::TagComponent>("Enemy");
        enemy->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        enemy->AddComponent<dae::LivesComponent>(1);

        return enemy;
    }

}

namespace UIFactory
{
    std::unique_ptr<dae::GameObject> CreateUI_Lives(
        const glm::vec2& pos,
        const std::string& texture)
    {
        auto ui = std::make_unique<dae::GameObject>();

        auto* comp = ui->AddComponent<dae::LivesDisplayComponent>(3);
        comp->SetTexture(texture);

        ui->SetPosition(pos.x, pos.y);

        return ui;
    }

    std::unique_ptr<dae::GameObject> CreateUI_Score(
        std::shared_ptr<dae::Font> font,
        const glm::vec2& pos)
    {
        auto ui = std::make_unique<dae::GameObject>();

        ui->AddComponent<dae::ScoreDisplayComponent>(font);
        ui->SetPosition(pos.x, pos.y);

        return ui;
    }
}