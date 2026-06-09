#include "Factory.h"
#include "GameObject.h"
#include "InputManager.h"
#include "GameCommands.h"
#include "GameComponents.h"
#include "EnemyComponents.h"
#include <SDL3/SDL_keycode.h>

namespace ActorFactory
{
    std::unique_ptr<dae::GameObject> CreatePlayer(dae::InputManager& input, const glm::vec2& pos, bool keyboardInput, bool gamepadInput)
    {
        auto player = std::make_unique<dae::GameObject>();
        player->AddComponent<dae::RenderComponent>()->SetTexture("Images/player.png", true);
        player->SetPosition(pos.x, pos.y);
        player->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Player);

        auto* collider = player->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = player->AddComponent<dae::DamageManager>();
        auto* lives = player->AddComponent<dae::LivesComponent>(3);
        player->AddComponent<dae::ScoreComponent>();
        player->AddComponent<dae::ShootComponent>(0.25f);

        float speed = 150.f;
        // Keyboard bindings
        if (keyboardInput)
        {
            input.BindCommand(SDLK_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
            input.BindCommand(SDLK_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
            input.BindCommand(SDLK_E, dae::KeyState::Up, std::make_unique<dae::ShootCommand>(player.get()));
        }

        // Gamepad bindings
        if (gamepadInput)
        {
            input.BindGamepadCommand(dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
            input.BindGamepadCommand(dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
            input.BindGamepadCommand(dae::GAMEPAD_A, dae::KeyState::Pressed, std::make_unique<dae::ShootCommand>(player.get()));
        }

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->AddThreat(dae::TagComponent::Tags::Enemy);
        return player;
    }

    std::unique_ptr<dae::GameObject> CreateEnemy(EnemyType enemyType, const glm::vec2& pos)
    {
        auto enemy = std::make_unique<dae::GameObject>();
        enemy->SetPosition(pos.x, pos.y);
        enemy->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Enemy);
        enemy->AddComponent<dae::EnemyComponent>(enemyType);

        auto* render = enemy->AddComponent<dae::RenderComponent>();
        int livesAmount{ 1 };

        switch (enemyType)
        {
        case EnemyType::Bee:
            render->SetTexture("Images/enemy_bees_1.png", true);
            livesAmount = 1;
            break;

        case EnemyType::Butterfly:
            render->SetTexture("Images/enemy_butterfly_1.png", true);
            livesAmount = 1;
            break;

            case EnemyType::BossGalaga:
            render->SetTexture("Images/enemy_butterfly_1.png", true);
            livesAmount = 2;
            break;
        }

        auto* collider = enemy->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = enemy->AddComponent<dae::DamageManager>();
        auto* lives = enemy->AddComponent<dae::LivesComponent>(livesAmount);
        enemy->AddComponent<EnemyStateComponent>();

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

    std::unique_ptr<dae::GameObject> CreateUI_Text(std::shared_ptr<dae::Font> font, const glm::vec2& pos, std::string text, const SDL_Color& color)
    {
        auto ui = std::make_unique<dae::GameObject>();
        auto textComp = ui->AddComponent<dae::TextComponent>(text, font);
        textComp->SetColor(color);
        ui->SetPosition(pos.x, pos.y);
        return ui;
    }
}