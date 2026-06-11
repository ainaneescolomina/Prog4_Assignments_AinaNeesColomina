#include "NameEntryState.h"
#include "HighscoreState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameStateManager.h"
#include "HighscoreManager.h"
#include "GameObject.h"
#include "Components.h"
#include "Factory.h"

#include "ServiceLocator.h"
#include "SdlSoundSystem.h"

void dae::NameEntryState::OnEnter()
{
    auto& input = dae::InputManager::GetInstance();
    // keyborad bindings
    input.BindCommand(SDLK_A, dae::KeyState::Down, std::make_unique<CycleLetterCommand>(this, -1));
    input.BindCommand(SDLK_D, dae::KeyState::Down, std::make_unique<CycleLetterCommand>(this, 1));
    input.BindCommand(SDLK_SPACE, dae::KeyState::Down, std::make_unique<ConfirmLetterCommand>(this));
    // gamepad bindings
    input.BindGamepadCommand(dae::GAMEPAD_DPAD_UP, KeyState::Down, std::make_unique<CycleLetterCommand>(this, -1));
    input.BindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, KeyState::Down, std::make_unique<CycleLetterCommand>(this, 1));
    input.BindGamepadCommand(dae::GAMEPAD_A, KeyState::Down, std::make_unique<ConfirmLetterCommand>(this));

    m_pScene = &dae::SceneManager::GetInstance().CreateScene();

    auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
    dae::servicelocator::register_sound_system(std::move(soundSystem));
    auto& sound = dae::servicelocator::get_sound_system();

    // preload sounds
    sound.Load(0, "Data/Sounds/UIConfirm.mp3");

    // 1. Background Visual Base
    auto background = std::make_unique<dae::GameObject>();
    background->AddComponent<dae::RenderComponent>()->SetTexture("Images/background.png");
    m_pScene->Add(std::move(background));

    auto fontTitle = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 36);
    auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);

    // 2. Heading Prompts
    auto titleObj = std::make_unique<dae::GameObject>();
    titleObj->AddComponent<dae::TextComponent>("-- Enter Initials --", fontTitle, SDL_Color{ 255, 0, 0, 255 });
    titleObj->SetPosition(300.f, 275.f);
    m_pScene->Add(std::move(titleObj));

    // 3. Spawning the Interactive Letter Container Object
    auto lettersObj = std::make_unique<dae::GameObject>();
    m_pNameEntryText = lettersObj->AddComponent<dae::TextComponent>("[A] A  A", fontTitle, SDL_Color{ 255, 255, 0, 255 });
    lettersObj->SetPosition(425.f, 325.f);
    m_pScene->Add(std::move(lettersObj));

    auto instructions = UIFactory::CreateUI_Text(fontSmall, { 15.f, 850.f }, "A/D to Cycle Letters - Space to Confirm");
    m_pScene->Add(std::move(instructions));
}

void dae::NameEntryState::OnExit()
{
    auto& input = dae::InputManager::GetInstance();
    input.UnbindCommand(SDLK_A, dae::KeyState::Down);
    input.UnbindCommand(SDLK_D, dae::KeyState::Down);
    input.UnbindCommand(SDLK_SPACE, dae::KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_UP, KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, KeyState::Down);
    input.UnbindGamepadCommand(dae::GAMEPAD_A, KeyState::Down);

    m_pScene->RemoveAll();
}

std::unique_ptr<dae::GameState> dae::NameEntryState::Update(float)
{
    return nullptr;
}

void dae::NameEntryState::CycleLetter(int change)
{
    char currentLetter = m_name[m_editIdx];
    currentLetter = static_cast<char>(currentLetter + change);

    // wrap current letter to get an infinite loop
    if (currentLetter > 'Z') currentLetter = 'A';
    if (currentLetter < 'A') currentLetter = 'Z';

    m_name[m_editIdx] = currentLetter;
    UpdateNameEntryText();
}

void dae::NameEntryState::ConfirmLetter()
{
    m_editIdx++;

    if (m_editIdx < 3)
    {
        UpdateNameEntryText();
    }
    else
    {
        // Full named confirmed

        // Save the score with the name
        HighscoreManager manager{};
        manager.SaveNewHighscore("Data/highscores.txt", m_name, m_finalScore);

        // Change to Highscore menu
        dae::GameStateManager::GetInstance().ChangeState(std::make_unique<dae::HighscoreState>());
    }
}

void dae::NameEntryState::UpdateNameEntryText()
{
    std::string nameTmp = "";
    for (int i = 0; i < 3; ++i)
    {
        if (i == m_editIdx)
        {
            // If the letter is selected add brakets "[A]"
            nameTmp += "[";
            nameTmp += m_name[i];
            nameTmp += "] ";
        }
        else
        {
            nameTmp += " ";
            nameTmp += m_name[i];
            nameTmp += "  ";
        }
    }

    m_pNameEntryText->SetText(nameTmp);
}

void dae::CycleLetterCommand::Execute(float)
{
    m_pState->CycleLetter(m_direction);
}

void dae::ConfirmLetterCommand::Execute(float)
{
    auto& sound = dae::servicelocator::get_sound_system();
    sound.Play(0, 0.1f);
    m_pState->ConfirmLetter();
}
