#include "CoOpPlayState.h"
#include "HighscoreState.h"
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

void dae::CoOpPlayState::OnEnter()
{
	InputManager::GetInstance().BindCommand(SDLK_F2, KeyState::Down, std::make_unique<MuteToggleCommand>());

	m_pScene = &SceneManager::GetInstance().CreateScene();

	auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
	dae::servicelocator::register_sound_system(std::move(soundSystem));
	auto& sound = dae::servicelocator::get_sound_system();

	// preload sounds
	sound.Load(0, "Data/Sounds/GameStart.mp3");
	sound.Load(1, "Data/Sounds/PlayerShoot.mp3");
	sound.Load(2, "Data/Sounds/PlayerDies.mp3");
	sound.Load(3, "Data/Sounds/EnemyDies.mp3");
	sound.Load(4, "Data/Sounds/BossDies.mp3");
	sound.Load(5, "Data/Sounds/TractorBeam.mp3");

	///////////////
	auto font = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 36);
	auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Fonts/Silkscreen-Regular.ttf", 18);
	auto& input = dae::InputManager::GetInstance();

	// --- UI ---
	auto background = std::make_unique<dae::GameObject>();
	background->AddComponent<dae::RenderComponent>()->SetTexture("Images/background.png");
	m_pScene->Add(std::move(background));

	//auto highscoreTitle = UIFactory::CreateUI_Text(font, { 750, 200 }, "High Score", { 255, 0, 0, 255 });
	//m_pScene->Add(std::move(highscoreTitle));
	//auto highscoreScore = UIFactory::CreateUI_Text(font, { 850, 250 }, "00000");
	//m_pScene->Add(std::move(highscoreScore));

	// --- MANAGERS ---
	m_pBulletSpawner = std::make_unique<dae::BulletSpawner>(*m_pScene);
	m_pLevelManager = std::make_unique<LevelManager>(*m_pScene);
	m_pGameStats = std::make_unique<GameStatsManager>();

	// --- ACTORS ---
	// Player 1
	auto player1 = ActorFactory::CreatePlayer(input, { 450, 650 }, false, true, 0);

	// Player 2
	auto player2 = ActorFactory::CreatePlayer(input, { 450, 650 }, true, true, 1);

	// UI - REVISAR
	auto scoreTitle = UIFactory::CreateUI_Text(font, { 850, 400 }, "2 Up", { 255, 0, 0, 255 });
	m_pScene->Add(std::move(scoreTitle));
	auto scoreUI = UIFactory::CreateUI_Score(font, { 850, 450 });

	auto livesUI1 = UIFactory::CreateUI_Lives({ 800, 600 }, "Images/player.png");
	auto livesUI2 = UIFactory::CreateUI_Lives({ 800, 650 }, "Images/player.png");

	// Observer / Subject
	auto* score = player1->GetComponent<dae::ScoreComponent>();
	//score->GetSubject().AddObserver(&m_pWinAchievement);

	auto* lives1 = player1->GetComponent<dae::LivesComponent>();
	lives1->GetSubject().AddObserver(livesUI1->GetComponent<dae::LivesDisplayComponent>());
	auto* lives2 = player2->GetComponent<dae::LivesComponent>();
	lives2->GetSubject().AddObserver(livesUI2->GetComponent<dae::LivesDisplayComponent>());

	score->GetSubject().AddObserver(scoreUI->GetComponent<dae::ScoreDisplayComponent>());

	auto* shoot1 = player1->GetComponent<dae::ShootComponent>();
	shoot1->GetSubject().AddObserver(m_pBulletSpawner.get());
	auto* shoot2 = player2->GetComponent<dae::ShootComponent>();
	shoot2->GetSubject().AddObserver(m_pBulletSpawner.get());

	m_pBulletSpawner->GetSubject().AddObserver(m_pGameStats.get());

	// --- GAMEPLAY ---
	m_pLevelManager->SetPlayerScore(score);
	m_pLevelManager->SetBulletSpawner(m_pBulletSpawner.get());
	m_pLevelManager->SetGameStatsManager(m_pGameStats.get());
	m_pLevelManager->SpawnWave();
	lives1->GetSubject().AddObserver(this);
	lives2->GetSubject().AddObserver(this);

	// Add to scene
	m_pScene->Add(std::move(player1));
	m_pScene->Add(std::move(player2));
	m_pScene->Add(std::move(livesUI1));
	m_pScene->Add(std::move(livesUI2));
	m_pScene->Add(std::move(scoreUI));

	// Start Game
	InputManager::GetInstance().BindCommand(SDLK_F1, KeyState::Down, std::make_unique<SkipLevelCommand>(m_pLevelManager.get()));
	sound.Play(0, 0.15f);
}

void dae::CoOpPlayState::OnExit()
{
	InputManager::GetInstance().UnbindCommand(SDLK_F1, KeyState::Down);
	InputManager::GetInstance().UnbindCommand(SDLK_F2, KeyState::Down);
	dae::InputManager::GetInstance().ClearAllBindings();
	m_pScene->RemoveAll();
}

std::unique_ptr<dae::GameState> dae::CoOpPlayState::Update(float delta_time)
{
	m_pLevelManager->Update(delta_time);

	if (m_playerDied >= 2)
	{
		return std::make_unique<HighscoreState>();
	}

	return nullptr;
}

void dae::CoOpPlayState::Notify(Event event, void* sender)
{
	if (event.id == make_sdbm_hash("ActorDied"))
	{
		auto* otherObj = static_cast<GameObject*>(sender);
		auto* otherTag = otherObj->GetComponent<TagComponent>();

		if (!otherTag) return;
		if (otherTag->GetTag() == TagComponent::Tags::Player)
		{
			++m_playerDied;
		}
	}
	else if (event.id == make_sdbm_hash("UpdateLives"))
	{
		auto& sound = dae::servicelocator::get_sound_system();
		sound.Play(2, 0.5f);
	}
}