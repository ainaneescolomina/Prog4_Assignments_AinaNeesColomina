#include "GameOverState.h"
#include "NameEntryState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"

#include "Factory.h"

#include <iostream> 
#include <string> 

void dae::GameOverState::OnEnter()
{
    dae::InputManager::GetInstance().BindCommand(SDLK_SPACE, dae::KeyState::Down, std::make_unique<OpenHighscoreMenuCommand>(m_score));

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 36);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);

    // --- UI ---
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("Images/background.png");
    m_pScene->Add(std::move(background));

    auto title = std::make_unique<dae::GameObject>();
    title->AddComponent<dae::TextComponent>("-- Results --", font, SDL_Color{ 255, 0, 0, 255 });
    title->SetPosition(350.f, 275.f);
    m_pScene->Add(std::move(title));

    auto shotsFiredTitle = std::make_unique<dae::GameObject>();
    shotsFiredTitle->AddComponent<dae::TextComponent>("Shots Fired:", font, SDL_Color{255, 255, 0, 255});
    shotsFiredTitle->SetPosition(250.f, 350.f);
    m_pScene->Add(std::move(shotsFiredTitle));

    auto shotsFiredStat = std::make_unique<dae::GameObject>();
    shotsFiredStat->AddComponent<dae::TextComponent>(std::to_string(m_shotsFired), font, SDL_Color{ 255, 255, 0, 255 });
    shotsFiredStat->SetPosition(675.f, 350.f);
    m_pScene->Add(std::move(shotsFiredStat));

    auto numberHitsTitle = std::make_unique<dae::GameObject>();
    numberHitsTitle->AddComponent<dae::TextComponent>("Number of Hits:", font, SDL_Color{ 255, 255, 0, 255 });
    numberHitsTitle->SetPosition(250.f, 400.f);
    m_pScene->Add(std::move(numberHitsTitle));

    auto numberHitsStat = std::make_unique<dae::GameObject>();
    numberHitsStat->AddComponent<dae::TextComponent>(std::to_string(m_numberHits), font, SDL_Color{ 255, 255, 0, 255 });
    numberHitsStat->SetPosition(675.f, 400.f);
    m_pScene->Add(std::move(numberHitsStat));

    auto ratioTitle = std::make_unique<dae::GameObject>();
    ratioTitle->AddComponent<dae::TextComponent>("Hit Miss Ratio:", font);
    ratioTitle->SetPosition(250.f, 450.f);
    m_pScene->Add(std::move(ratioTitle));

    auto ratioStat = std::make_unique<dae::GameObject>();
    ratioStat->AddComponent<dae::TextComponent>("%"+std::to_string(m_ratio), font, SDL_Color{255, 255, 0, 255});
    ratioStat->SetPosition(675.f, 450.f);
    m_pScene->Add(std::move(ratioStat));

    auto menuInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "Press Space or button A to open the Highscore Menu");
    m_pScene->Add(std::move(menuInstructions));
}

void dae::GameOverState::OnExit()
{
    m_pScene->RemoveAll();
    dae::InputManager::GetInstance().UnbindCommand(SDLK_SPACE, dae::KeyState::Down);
}

std::unique_ptr<dae::GameState> dae::GameOverState::Update(float)
{
    return nullptr;
}

void dae::OpenHighscoreMenuCommand::Execute(float)
{
    dae::GameStateManager::GetInstance().ChangeState(std::make_unique<dae::NameEntryState>(m_score));
}
