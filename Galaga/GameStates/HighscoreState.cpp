#include "HighscoreState.h"
#include "MenuState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"
#include "HighscoreManager.h"

#include "Factory.h"

void dae::HighscoreState::OnEnter()
{
    dae::InputManager::GetInstance().BindCommand(SDLK_SPACE, dae::KeyState::Down, std::make_unique<OpenMenuCommand>());
    dae::InputManager::GetInstance().BindGamepadCommand(0, dae::GAMEPAD_A, dae::KeyState::Down, std::make_unique<OpenMenuCommand>());

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto font = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 36);
    auto fontLine = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 30);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);

    // --- UI ---
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("background.png");
    m_pScene->Add(std::move(background));

    auto title = std::make_unique<dae::GameObject>();
    title->AddComponent<dae::TextComponent>("-- High Scores --", font, SDL_Color{ 0, 255, 255, 255 });
    title->SetPosition(325.f, 275.f);
    m_pScene->Add(std::move(title));

    // Ranking
    HighscoreManager manager{};
    std::vector<dae::HighscoreData> scores = manager.LoadHighscores("Data/highscores.txt");

    float startY = 350.f;
    float spacing = 50.f;

    for (int i = 0; i < static_cast<int>(scores.size()); ++i)
    {
        std::string placementText = std::to_string(i + 1) + ".  " + scores[i].name + "   " + std::to_string(scores[i].score);

        auto rowObj = std::make_unique<dae::GameObject>();
        rowObj->AddComponent<dae::TextComponent>(placementText, fontLine, SDL_Color{ 255, 255, 255, 255 });
        rowObj->SetPosition(325.f, startY + (i * spacing));
        m_pScene->Add(std::move(rowObj));
    }

    auto menuInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "Press Space or button A to return to the Menu");
    m_pScene->Add(std::move(menuInstructions));
}

void dae::HighscoreState::OnExit()
{
    m_pScene->RemoveAll();
    dae::InputManager::GetInstance().UnbindCommand(SDLK_SPACE, dae::KeyState::Down);
    dae::InputManager::GetInstance().UnbindGamepadCommand(dae::GAMEPAD_A, dae::KeyState::Down);
}

std::unique_ptr<dae::GameState> dae::HighscoreState::Update(float)
{
	return nullptr;
}

void dae::OpenMenuCommand::Execute(float, float)
{
    dae::GameStateManager::GetInstance().ChangeState(std::make_unique<dae::MenuState>());
}
