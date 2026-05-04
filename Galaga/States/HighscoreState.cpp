#include "HighscoreState.h"
#include "MenuState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"

void dae::HighscoreState::OnEnter()
{
    dae::InputManager::GetInstance().BindCommand(SDLK_SPACE, dae::KeyState::Down, std::make_unique<ReturnGameCommand>());

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto font = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 36);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 18);

    // --- UI ---
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("background.png");
    m_pScene->Add(std::move(background));

    auto gameModeSingle = std::make_unique<dae::GameObject>();
    gameModeSingle->AddComponent<dae::TextComponent>("1 Player", font);
    gameModeSingle->SetPosition(300.f, 500.f);
    m_pScene->Add(std::move(gameModeSingle));

    auto gameModeCoop = std::make_unique<dae::GameObject>();
    gameModeCoop->AddComponent<dae::TextComponent>("2 Players Co-op", font);
    gameModeCoop->SetPosition(300.f, 550.f);
    m_pScene->Add(std::move(gameModeCoop));

    auto gameModeVersus = std::make_unique<dae::GameObject>();
    gameModeVersus->AddComponent<dae::TextComponent>("2 Players Versus", font);
    gameModeVersus->SetPosition(300.f, 600.f);
    m_pScene->Add(std::move(gameModeVersus));
}

void dae::HighscoreState::OnExit()
{
    m_pScene->RemoveAll();
    dae::InputManager::GetInstance().UnbindCommand(SDLK_SPACE, dae::KeyState::Down);
}

std::unique_ptr<dae::GameState> dae::HighscoreState::Update(float)
{
	return std::unique_ptr<dae::GameState>();
}

void dae::ReturnGameCommand::Execute(float)
{
    dae::GameStateManager::GetInstance().ChangeState(std::make_unique<dae::MenuState>());
}
