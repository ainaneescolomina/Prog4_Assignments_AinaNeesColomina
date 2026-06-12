#include "Factory.h"
#include "GameObject.h"
#include "InputManager.h"
#include "GameCommands.h"
#include "GameComponents.h"
#include "EnemyComponents.h"
#include "EnemyIdleState.h"
#include <SDL3/SDL_keycode.h>

namespace ActorFactory
{
    std::unique_ptr<dae::GameObject> CreatePlayer(dae::InputManager& input, const glm::vec2& pos, bool keyboardInput, bool gamepadInput, int gamepadIdx)
    {
        auto player = std::make_unique<dae::GameObject>();
        player->AddComponent<dae::AnimatedRenderComponent>(1, 1, 0.f)->SetTexture("Images/player.png", true);
        player->SetPosition(pos.x, pos.y);
        player->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Player);

        auto* collider = player->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = player->AddComponent<dae::DamageManager>();
        auto* lives = player->AddComponent<dae::LivesComponent>(4, dae::LivesComponent::DeathAction::Deactivate, 1.f);
        player->AddComponent<dae::ScoreComponent>();
        player->AddComponent<dae::ShootComponent>(0.1f);
        player->AddComponent<dae::ScreenBoundsComponent>(25.f, 775.f, 0.f, 915.f);

        auto* movement = player->AddComponent<dae::MovementComponent>(pos.x, pos.y);

        float speed = 215.f;
        // Keyboard bindings
        if (keyboardInput)
        {
            input.BindCommand(SDLK_A, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
            input.BindCommand(SDLK_D, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
            input.BindCommand(SDLK_SPACE, dae::KeyState::Up, std::make_unique<dae::ShootCommand>(player.get()));
        }

        // Gamepad bindings
        if (gamepadInput)
        {
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), -speed, 0.f));
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_A, dae::KeyState::Pressed, std::make_unique<dae::ShootCommand>(player.get()));
        
            input.BindAxisCommand(gamepadIdx, std::make_unique<dae::MoveCommand>(player.get(), speed, 0.f));
        }

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->GetSubject().AddObserver(movement);
        damage->AddThreat(dae::TagComponent::Tags::Enemy);
        damage->AddThreat(dae::TagComponent::Tags::EnemyBullet);
        damage->AddThreat(dae::TagComponent::Tags::TractorBeam);

        return player;
    }

    std::unique_ptr<dae::GameObject> CreateGalagaPlayer(dae::InputManager& input, const glm::vec2& pos, bool keyboardInput, bool gamepadInput, int gamepadIdx)
    {
        auto player = std::make_unique<dae::GameObject>();
        player->AddComponent<dae::AnimatedRenderComponent>(1, 2, 1.f)->SetTexture("Images/enemy_galaga.png", true);
        player->SetPosition(pos.x, pos.y);
        player->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Enemy);

        auto* enemyComp = player->AddComponent<dae::EnemyComponent>(EnemyType::BossGalaga);
        glm::vec2 formationPos{ pos.x, pos.y };
        enemyComp->SetFormationPosition(formationPos);

        auto* collider = player->AddComponent<dae::ColliderComponent>(48.f, 48.f);
        auto* damage = player->AddComponent<dae::DamageManager>();
        auto* lives = player->AddComponent<dae::LivesComponent>(2, dae::LivesComponent::DeathAction::None, 0.5f);
        
        auto* stateComp = player->AddComponent<EnemyStateComponent>();
        stateComp->SetState(std::make_unique<dae::EnemyIdleState>());
        player->AddComponent<dae::ShootComponent>(0.1f);

        // Keyboard bindings
        if (keyboardInput)
        {
            input.BindCommand(SDLK_A, dae::KeyState::Pressed, std::make_unique<dae::VersusBossDiveCommand>(player.get()));
            input.BindCommand(SDLK_D, dae::KeyState::Pressed, std::make_unique<dae::VersusBossTractBeamCommand>(player.get()));
            input.BindCommand(SDLK_SPACE, dae::KeyState::Up, std::make_unique<dae::ShootCommand>(player.get()));
        }

        // Gamepad bindings
        if (gamepadInput)
        {
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed, std::make_unique<dae::VersusBossDiveCommand>(player.get()));
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed, std::make_unique<dae::VersusBossTractBeamCommand>(player.get()));
            input.BindGamepadCommand(gamepadIdx, dae::GAMEPAD_A, dae::KeyState::Pressed, std::make_unique<dae::ShootCommand>(player.get()));
        }

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->GetSubject().AddObserver(enemyComp);
        lives->GetSubject().AddObserver(enemyComp);
        damage->AddThreat(dae::TagComponent::Tags::Bullet);

        return player;
    }

    std::unique_ptr<dae::GameObject> CreateEnemy(EnemyType enemyType, const glm::vec2& pos)
    {
        auto enemy = std::make_unique<dae::GameObject>();
        enemy->SetPosition(pos.x, pos.y);
        enemy->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::Enemy);
        
        auto* enemyComp = enemy->AddComponent<dae::EnemyComponent>(enemyType);
        glm::vec2 formationPos{ pos.x, pos.y };
        enemyComp->SetFormationPosition(formationPos);

        auto* render = enemy->AddComponent<dae::AnimatedRenderComponent>(1, 2, 1.f);
        int livesAmount{ 1 };

        switch (enemyType)
        {
        case EnemyType::Bee:
            render->SetTexture("Images/enemy_bee.png", true);
            livesAmount = 1;
            break;

        case EnemyType::Butterfly:
            render->SetTexture("Images/enemy_butterfly.png", true);
            livesAmount = 1;
            break;

            case EnemyType::BossGalaga:
            render->SetTexture("Images/enemy_galaga.png", true);
            livesAmount = 2;
            break;
        }

        auto* collider = enemy->AddComponent<dae::ColliderComponent>(45.f, 45.f);
        auto* damage = enemy->AddComponent<dae::DamageManager>();
        auto* lives = enemy->AddComponent<dae::LivesComponent>(livesAmount, dae::LivesComponent::DeathAction::None);
        enemy->AddComponent<EnemyStateComponent>();
        enemy->AddComponent<dae::ShootComponent>(0.1f);

        // Observer / Subject
        collider->GetSubject().AddObserver(damage);
        damage->GetSubject().AddObserver(lives);
        damage->GetSubject().AddObserver(enemyComp);
        lives->GetSubject().AddObserver(enemyComp);
        damage->AddThreat(dae::TagComponent::Tags::Bullet);
        return enemy;
    }

    std::unique_ptr<dae::GameObject> CreateTractorBeam(dae::GameObject* enemy)
    {
        // Create the Beam GameObject
        auto beam = std::make_unique<dae::GameObject>();
        beam->AddComponent<dae::AnimatedRenderComponent>(1, 3, 0.5f)->SetTexture("Images/tractor_beam.png", true);

        // Position it directly underneath the Boss
        beam->SetPosition(0.f, 120.f);
        beam->AddComponent<dae::TagComponent>(dae::TagComponent::Tags::TractorBeam);

        beam->AddComponent<dae::ColliderComponent>(144.f, 240.f);

        // deactivated initially
        beam->SetActive(false);
        beam->SetParent(enemy, false);

        auto* enemyComp = enemy->GetComponent<dae::EnemyComponent>();
        enemyComp->SetBeamObject(beam.get());

        return beam;
    }
}

namespace UIFactory
{
    std::unique_ptr<dae::GameObject> CreateUI_Lives(const glm::vec2& pos, const std::string& texture)
    {
        auto ui = std::make_unique<dae::GameObject>();
        auto* comp = ui->AddComponent<dae::LivesDisplayComponent>(4);
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