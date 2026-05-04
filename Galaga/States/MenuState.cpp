#include "MenuState.h"
#include "PlayState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"

void dae::MenuState::OnEnter()
{
    InputManager::GetInstance().BindCommand(SDLK_SPACE, KeyState::Down, std::make_unique<StartGameCommand>());

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto font = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 36);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 18);

    // --- UI ---
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("background.png");
    m_pScene->Add(std::move(background));

    auto logo = std::make_unique<dae::GameObject>();
    logo->AddComponent<dae::RenderComponent>()->SetTexture("logo.png");
    logo->SetPosition(350.f, 217.f);
    m_pScene->Add(std::move(logo));

    auto gameModeSingle = UIFactory::CreateUI_Text(font, { 300.f, 500.f }, "1 Player");
    m_pScene->Add(std::move(gameModeSingle));

    auto gameModeCoop = UIFactory::CreateUI_Text(font, { 300.f, 550.f }, "2 Players Co-op");
    m_pScene->Add(std::move(gameModeCoop));

    auto gameModeVersus = UIFactory::CreateUI_Text(font, { 300.f, 600.f }, "2 Players Versus");
    m_pScene->Add(std::move(gameModeVersus));

    auto movementInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 750.f }, "Move Player with WASD or D-Pad, shoot bullets with E or button A");
    m_pScene->Add(std::move(movementInstructions));

    auto liveInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 800.f }, "You lose lives by colliding against enemies");
    m_pScene->Add(std::move(liveInstructions));

    auto scoreInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "You gain points by shooting enemies");
    m_pScene->Add(std::move(scoreInstructions));
}

void dae::MenuState::OnExit()
{
    m_pScene->RemoveAll();
    InputManager::GetInstance().UnbindCommand(SDLK_SPACE, KeyState::Down);
}

std::unique_ptr<dae::GameState> dae::MenuState::Update(float)
{
    return nullptr;
}

void dae::StartGameCommand::Execute(float)
{
    GameStateManager::GetInstance().ChangeState(std::make_unique<PlayState>());
}
