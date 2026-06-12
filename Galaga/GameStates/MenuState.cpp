#include "MenuState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"
#include "Components.h"

#include "SinglePlayerState.h"
#include "CoOpPlayState.h"
#include "HighscoreState.h"

#include "Factory.h"

#include "ServiceLocator.h"
#include "SdlSoundSystem.h"

void dae::MenuState::OnEnter()
{
    auto& input = InputManager::GetInstance();

    // keyborad bindings
    input.BindCommand(SDLK_W, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, -1));
    input.BindCommand(SDLK_S, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, 1));
    input.BindCommand(SDLK_SPACE, KeyState::Down, std::make_unique<ConfirmSelectionCommand>(this));

    // gamepad bindings
    input.BindGamepadCommand(0, dae::GAMEPAD_DPAD_UP, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, -1));
    input.BindGamepadCommand(0, dae::GAMEPAD_DPAD_DOWN, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, 1));
    input.BindGamepadCommand(0, dae::GAMEPAD_A, KeyState::Down, std::make_unique<ConfirmSelectionCommand>(this));

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
    dae::servicelocator::register_sound_system(std::move(soundSystem));
    auto& sound = dae::servicelocator::get_sound_system();

    // preload sounds
    sound.Load(0, "Data/Sounds/UIConfirm.mp3");

    auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 36);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);

    // --- UI ---
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("Images/background.png");
    m_pScene->Add(std::move(background));

    auto logo = std::make_unique<dae::GameObject>();
    logo->AddComponent<dae::RenderComponent>()->SetTexture("Images/logo.png");
    logo->SetPosition(350.f, 217.f);
    m_pScene->Add(std::move(logo));

    std::vector<std::string> gameModes = { "1 Player", "2 Players Co-op", "2 Players Versus", "Highscores", "Quit Game" };
    float startY = 400.f;
    float spacing = 50.f;

    for (int i = 0; i < static_cast<int>(gameModes.size()); ++i)
    {
        auto textObj = std::make_unique<dae::GameObject>();
        auto* textComp = textObj->AddComponent<dae::TextComponent>(gameModes[i], font);
        textObj->SetPosition(300.f, startY + (i * spacing));

        m_pMenuTexts.push_back(textComp);
        m_pScene->Add(std::move(textObj));
    }

    UpdateModeSelection();

    auto liveInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 750.f }, "Select: W/S or D-Pad  |  Confirm: Space/Button A");
    m_pScene->Add(std::move(liveInstructions));

    auto scoreInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 800.f }, "Move: WASD/Arrows/D-Pad  |  Shoot: Space/Button A ");
    m_pScene->Add(std::move(scoreInstructions));

    auto menuInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "Gameplay: Lose lives on collision, gain points by shooting threats.");
    m_pScene->Add(std::move(menuInstructions));
}

void dae::MenuState::OnExit()
{
    m_pScene->RemoveAll();
    m_pMenuTexts.clear();

    auto& input = InputManager::GetInstance();
    input.UnbindCommand(SDLK_W, KeyState::Down);
    input.UnbindCommand(SDLK_S, KeyState::Down);
    input.UnbindCommand(SDLK_SPACE, KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_UP, KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_A, KeyState::Down);
}

std::unique_ptr<dae::GameState> dae::MenuState::Update(float)
{
    return nullptr;
}

void dae::MenuState::MoveSelection(int direction)
{
    m_selectedIdx += direction;
    int maxCount = 5;

    if (m_selectedIdx >= maxCount) m_selectedIdx = 0;
    if (m_selectedIdx < 0) m_selectedIdx = maxCount - 1;

    UpdateModeSelection();
}

void dae::MenuState::ConfirmSelection()
{
    switch (static_cast<GameMode>(m_selectedIdx))
    {
    case GameMode::SinglePlayer:
        GameStateManager::GetInstance().ChangeState(std::make_unique<SinglePlayerState>());
        break;

    case GameMode::CoOp:
        GameStateManager::GetInstance().ChangeState(std::make_unique<CoOpPlayState>());
        break;

    case GameMode::Versus:
    default:
        // Stubs for un-implemented multiplayer elements so they don't crash
        break;

    case GameMode::Highscore:
        GameStateManager::GetInstance().ChangeState(std::make_unique<HighscoreState>());
        break;

    case GameMode::Quit:
        // Quit game
        SDL_Event quitEvent;
        quitEvent.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quitEvent);
        break;
    }
}

void dae::MenuState::UpdateModeSelection()
{
    SDL_Color white{ 255, 255, 255, 255 };
    SDL_Color red{ 255, 0, 0, 255 };

    // selected text = red
    // other text = white
    for (int i = 0; i < static_cast<int>(m_pMenuTexts.size()); ++i)
    {
        if (m_pMenuTexts[i])
            m_pMenuTexts[i]->SetColor(static_cast<int>(i) == m_selectedIdx ? red : white);
    }
}

void dae::NavigateMenuCommand::Execute(float, float)
{
    auto& sound = dae::servicelocator::get_sound_system();
    sound.Play(0, 0.1f);
    m_pMenu->MoveSelection(m_direction);
}

void dae::ConfirmSelectionCommand::Execute(float, float)
{
    m_pMenu->ConfirmSelection();
}
