#include "PlayState.h"
#include "HighscoreState.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "InputManager.h"
//#include "Commands.h"

#include "ServiceLocator.h"
#include "SdlSoundSystem.h"

// ---------------

#include "Factory.h"
#include "GameComponents.h"

void dae::PlayState::OnEnter()
{
	InputManager::GetInstance().BindCommand(SDLK_F1, KeyState::Down, std::make_unique<SkipLevelCommand>(this));

	m_pScene = &SceneManager::GetInstance().CreateScene();

	auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
	dae::servicelocator::register_sound_system(std::move(soundSystem));
	auto& sound = dae::servicelocator::get_sound_system();

	// preload sounds
	sound.load(0, "Data/Sounds/Fighter_Shot1.mp3");

	///////////////
	auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 36);
	auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);
	auto& input = dae::InputManager::GetInstance();

	// --- UI ---
	auto background = std::make_unique<dae::GameObject>();
	background->AddComponent<dae::RenderComponent>()->SetTexture("Images/background.png");
	m_pScene->Add(std::move(background));

	auto highscoreTitle = UIFactory::CreateUI_Text(font, { 750, 200 }, "High Score", { 255, 0, 0, 255 });
	m_pScene->Add(std::move(highscoreTitle));

	auto highscoreScore = UIFactory::CreateUI_Text(font, { 850, 250 }, "00000");
	m_pScene->Add(std::move(highscoreScore));

	// --- MANAGERS ---
	m_pBulletSpawner = std::make_unique<dae::BulletSpawner>(*m_pScene);
	m_pWaveSpawner = std::make_unique<WaveSpawner>(*m_pScene);

	// --- ACTORS ---
	// Player
	auto player = ActorFactory::CreatePlayer(input, { 450, 650 });

	// UI
	auto scoreTitle = UIFactory::CreateUI_Text(font, { 850, 400 }, "1 Up", { 255, 0, 0, 255 });
	m_pScene->Add(std::move(scoreTitle));
	auto scoreUI = UIFactory::CreateUI_Score(font, { 850, 450 });
	auto livesUI = UIFactory::CreateUI_Lives({ 800, 600 }, "Images/player.png");

	// Observer / Subject
	auto score = player->GetComponent<dae::ScoreComponent>();
	//score->GetSubject().AddObserver(&m_pWinAchievement);

	auto lives = player->GetComponent<dae::LivesComponent>();
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());

	score->GetSubject().AddObserver(scoreUI->GetComponent<dae::ScoreDisplayComponent>());
	player->GetComponent<dae::ShootComponent>()->GetSubject().AddObserver(m_pBulletSpawner.get());

	// --- GAMEPLAY ---
	m_pWaveSpawner->SetPlayerScore(score);
	m_pWaveSpawner->SpawnWave();
	lives->GetSubject().AddObserver(this);

	// Add to scene
	m_pScene->Add(std::move(player));
	m_pScene->Add(std::move(livesUI));
	m_pScene->Add(std::move(scoreUI));
}

void dae::PlayState::OnExit()
{
	InputManager::GetInstance().UnbindCommand(SDLK_F1, KeyState::Down);
	dae::InputManager::GetInstance().ClearAllBindings();
	m_pScene->RemoveAll();
}

std::unique_ptr<dae::GameState> dae::PlayState::Update(float delta_time)
{
	m_pWaveSpawner->Update(delta_time);

    if (m_playerDied) 
	{
        return std::make_unique<HighscoreState>();
    }

    return nullptr;
}

void dae::PlayState::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("ActorDied"))
	{
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTag = otherObj->GetComponent<TagComponent>();

		if (!otherTag) return;
		if (otherTag->GetTag() == TagComponent::Tags::Player)
		{
			m_playerDied = true;
		}
	}
}

void dae::PlayState::SkipLevel()
{
	m_pWaveSpawner->SkipLevel();
}

void dae::SkipLevelCommand::Execute(float)
{
	m_pPlayState->SkipLevel();
}
