#include "Factory.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Commands.h"
#include "Components.h"
#include <SDL3/SDL_keycode.h>

namespace ActorFactory
{
    std::unique_ptr<dae::GameObject> CreatePlayer(dae::InputManager& input, const glm::vec2& pos)
    {
        auto player = std::make_unique<dae::GameObject>();
        player->AddComponent<dae::RenderComponent>()->SetTexture("player.png");
        player->SetPosition(pos.x, pos.y);
        player->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Player);

        auto* collider = player->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = player->AddComponent<dae::DamageManager>();
        auto* lives = player->AddComponent<dae::LivesComponent>(3);
        player->AddComponent<dae::ScoreComponent>();
        player->AddComponent<dae::ShootComponent>(1.f);

        float speed = 75.f;
        // Keyboard bindings
        input.BindCommand(SDLK_W, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), 0.f, -speed));
        input.BindCommand(SDLK_S, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), 0.f, speed));
        input.BindCommand(SDLK_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
        input.BindCommand(SDLK_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
        input.BindCommand(SDLK_E, dae::KeyState::Up, std::make_unique<dae::ShootCommand>(player.get()));

        // Gamepad bindings
        input.BindGamepadCommand(dae::GAMEPAD_DPAD_UP, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), 0.f, -speed));
        input.BindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), 0.f, speed));
        input.BindGamepadCommand(dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
        input.BindGamepadCommand(dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
        input.BindGamepadCommand(dae::GAMEPAD_A, dae::KeyState::Pressed, std::make_unique<dae::ShootCommand>(player.get()));

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->AddThreat(dae::TagComponent::Tags::Enemy);
        return player;
    }

    std::unique_ptr<dae::GameObject> CreateEnemy(const glm::vec2& pos)
    {
        auto enemy = std::make_unique<dae::GameObject>();
        enemy->AddComponent<dae::RenderComponent>()->SetTexture("enemy_butterfly.png");
        enemy->SetPosition(pos.x, pos.y);
        enemy->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Enemy);
        auto* collider = enemy->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = enemy->AddComponent<dae::DamageManager>();
        auto* lives = enemy->AddComponent<dae::LivesComponent>(1);

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->AddThreat(dae::TagComponent::Tags::Bullet);
        return enemy;
    }
}

namespace UIFactory
{
    std::unique_ptr<dae::GameObject> CreateUI_Lives(const glm::vec2& pos, const std::string& texture)
    {
        auto ui = std::make_unique<dae::GameObject>();
        auto* comp = ui->AddComponent<dae::LivesDisplayComponent>(3);
        comp->SetTexture(texture);
        ui->SetPosition(pos.x, pos.y);
        return ui;
    }

    std::unique_ptr<dae::GameObject> CreateUI_Score(std::shared_ptr<dae::Font> font, const glm::vec2& pos)
    {
        auto ui = std::make_unique<dae::GameObject>();
        ui->AddComponent<dae::ScoreDisplayComponent>(font);
        ui->SetPosition(pos.x, pos.y);
        return ui;
    }
}