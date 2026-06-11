#include "PlayState.h"
#include "GameOverState.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "InputManager.h"
#include "GameCommands.h"

#include "ServiceLocator.h"
#include "SdlSoundSystem.h"

// ---------------

#include "Factory.h"
#include "GameComponents.h"

void dae::PlayState::OnEnter()
{
	InputManager::GetInstance().BindCommand(SDLK_F1, KeyState::Down, std::make_unique<SkipLevelCommand>(this));
	InputManager::GetInstance().BindCommand(SDLK_F2, KeyState::Down, std::make_unique<MuteToggleCommand>());

	m_pScene = &SceneManager::GetInstance().CreateScene();

	auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
	dae::servicelocator::register_sound_system(std::move(soundSystem));
	auto& sound = dae::servicelocator::get_sound_system();

	// preload sounds
	sound.Load(0, "Data/Sounds/PlayerShoot.mp3");
	sound.Load(1, "Data/Sounds/GameStart.mp3");

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
	m_pLevelManager = std::make_unique<LevelManager>(*m_pScene);
	m_pGameStats = std::make_unique<GameStatsManager>();

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
	auto shoot = player->GetComponent<dae::ShootComponent>();
	shoot->GetSubject().AddObserver(m_pBulletSpawner.get());
	shoot->GetSubject().AddObserver(m_pGameStats.get());

	m_pBulletSpawner->GetSubject().AddObserver(m_pGameStats.get());

	// --- GAMEPLAY ---
	m_pLevelManager->SetPlayerScore(score);
	m_pLevelManager->SetBulletSpawner(m_pBulletSpawner.get());
	m_pLevelManager->SetGameStats(m_pGameStats.get());
	m_pLevelManager->SpawnWave();
	lives->GetSubject().AddObserver(this);

	// Add to scene
	m_pScene->Add(std::move(player));
	m_pScene->Add(std::move(livesUI));
	m_pScene->Add(std::move(scoreUI));

	// Start Game
	sound.Play(1, 0.5f);
}

void dae::PlayState::OnExit()
{
	InputManager::GetInstance().UnbindCommand(SDLK_F1, KeyState::Down);
	InputManager::GetInstance().UnbindCommand(SDLK_F2, KeyState::Down);
	dae::InputManager::GetInstance().ClearAllBindings();
	m_pScene->RemoveAll();
}

std::unique_ptr<dae::GameState> dae::PlayState::Update(float delta_time)
{
	m_pLevelManager->Update(delta_time);

    if (m_playerDied) 
	{
        return std::make_unique<GameOverState>(
			m_pGameStats->GetShotsFired(), 
			m_pGameStats->GetHitsScored(), 
			m_pGameStats->GetHitMissRatio(), 
			m_pGameStats->GetFinalScore());
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
	m_pLevelManager->SkipLevel();
}

void dae::SkipLevelCommand::Execute(float)
{
	m_pPlayState->SkipLevel();
}
