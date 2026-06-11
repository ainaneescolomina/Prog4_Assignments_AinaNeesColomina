#include "MenuState.h"
#include "PlayState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"
#include "Components.h"

#include "Factory.h"

void dae::MenuState::OnEnter()
{
    auto& input = InputManager::GetInstance();

    // keyborad bindings
    input.BindCommand(SDLK_W, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, -1));
    input.BindCommand(SDLK_S, KeyState::Down, std::make_unique<NavigateMenuCommand>(this, 1));
    input.BindCommand(SDLK_SPACE, KeyState::Down, std::make_unique<ConfirmSelectionCommand>(this));

    // gamepad bindings
    input.BindGamepadCommand(dae::GAMEPAD_DPAD_UP, KeyState::Pressed, std::make_unique<NavigateMenuCommand>(this, -1));
    input.BindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, KeyState::Pressed, std::make_unique<NavigateMenuCommand>(this, 1));
    input.BindGamepadCommand(dae::GAMEPAD_A, KeyState::Pressed, std::make_unique<ConfirmSelectionCommand>(this));
    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

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

    std::vector<std::string> gameModes = { "1 Player", "2 Players Co-op", "2 Players Versus", "Quit Game" };
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
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_UP, KeyState::Pressed);
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, KeyState::Pressed);
    input.UnbindGamepadCommand(dae::GAMEPAD_A, KeyState::Pressed);
}

std::unique_ptr<dae::GameState> dae::MenuState::Update(float)
{
    return nullptr;
}

void dae::MenuState::MoveSelection(int direction)
{
    m_selectedIdx += direction;
    int maxCount = 4;

    if (m_selectedIdx >= maxCount) m_selectedIdx = 0;
    if (m_selectedIdx < 0) m_selectedIdx = maxCount - 1;

    UpdateModeSelection();
}

void dae::MenuState::ConfirmSelection()
{
    switch (static_cast<GameMode>(m_selectedIdx))
    {
    case GameMode::SinglePlayer:
        GameStateManager::GetInstance().ChangeState(std::make_unique<PlayState>());
        break;

    case GameMode::CoOp:
    case GameMode::Versus:
    default:
        // Stubs for un-implemented multiplayer elements so they don't crash
        break;

    case GameMode::Quit:
        //SDL_Event quitEvent;
        //quitEvent.type = SDL_QUIT;
        //SDL_PushEvent(&quitEvent);
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