#include "MenuState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"
#include "Components.h"

#include "SinglePlayerState.h"
#include "CoOpPlayState.h"
#include "VersusPlayState.h"
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

    auto liveInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 750.f }, "Select: W/S / D-Pad  |  Confirm: Space / Button A");
    m_pScene->Add(std::move(liveInstructions));

    auto scoreInstructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 800.f }, "", { 0, 255, 255, 255 });
    m_pDescriptionText = scoreInstructions->GetComponent<dae::TextComponent>();
    m_pScene->Add(std::move(scoreInstructions));

    auto instructionTopObj = UIFactory::CreateUI_Text(fontSmall, { 15.f, 825.f }, "", { 0, 255, 255, 255 });
    m_pInstructionTopText = instructionTopObj->GetComponent<dae::TextComponent>();
    m_pScene->Add(std::move(instructionTopObj));

    auto instructionBotObj = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "", { 0, 255, 255, 255 });
    m_pInstructionBotText = instructionBotObj->GetComponent<dae::TextComponent>();
    m_pScene->Add(std::move(instructionBotObj));

    UpdateModeSelection();
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
    default:
        GameStateManager::GetInstance().ChangeState(std::make_unique<SinglePlayerState>());
        break;

    case GameMode::CoOp:
        GameStateManager::GetInstance().ChangeState(std::make_unique<CoOpPlayState>());
        break;

    case GameMode::Versus:
        GameStateManager::GetInstance().ChangeState(std::make_unique<VersusPlayState>());
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

    if (m_pDescriptionText)
    {
        switch (static_cast<GameMode>(m_selectedIdx))
        {
        case GameMode::SinglePlayer:
            m_pDescriptionText->SetText("Single player against enemy waves");
            break;
        case GameMode::CoOp:
            m_pDescriptionText->SetText("Two-player cooperative mode");
            break;
        case GameMode::Versus:
            m_pDescriptionText->SetText("P1 flies the Starfighter, P2 plays as the Boss Galaga.");
            break;
        case GameMode::Highscore:
            m_pDescriptionText->SetText(" ");
            break;
        case GameMode::Quit:
            m_pDescriptionText->SetText(" ");
            break;
        }
    }

    if (m_pInstructionTopText && m_pInstructionBotText)
    {
        switch (static_cast<GameMode>(m_selectedIdx))
        {
        case GameMode::SinglePlayer:
            m_pInstructionTopText->SetText("Move: WASD / D-Pad / Joystick  |  Shoot: Space / Button A");
            m_pInstructionBotText->SetText(" ");
            break;

        case GameMode::CoOp:
            m_pInstructionTopText->SetText("P1 (Fighter) Move: D-Pad / Joystick  |  Shoot: Button A");
            m_pInstructionBotText->SetText("P2 (Fighter) Move: WASD / D-Pad / Joystick  |  Shoot: Space / Button A");
            break;

        case GameMode::Versus:
            m_pInstructionTopText->SetText("P1 (Fighter) Move: D-Pad / Joystick  |  Shoot: Button A");
            m_pInstructionBotText->SetText("P2 (Boss)    Dive: D-Pad Left  |  Tractor Beam: D-Pad Right  |  Shoot: Button A");
            break;

        case GameMode::Highscore:
            m_pInstructionTopText->SetText(" ");
            m_pInstructionBotText->SetText(" ");
            break;

        case GameMode::Quit:
            m_pInstructionTopText->SetText(" ");
            m_pInstructionBotText->SetText(" ");
            break;
        }
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
