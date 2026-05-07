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

void dae::PlayState::OnEnter()
{
	m_pScene = &SceneManager::GetInstance().CreateScene();

	auto soundSystem = std::make_unique<dae::SdlSoundSystem>();
	dae::servicelocator::register_sound_system(std::move(soundSystem));
	auto& sound = dae::servicelocator::get_sound_system();

	// preload sounds
	sound.load(0, "Data/Sounds/Fighter_Shot1.mp3");

	///////////////
	auto font = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 36);
	auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 18);
	auto& input = dae::InputManager::GetInstance();

	// --- UI ---
	auto background = std::make_unique<dae::GameObject>();
	background->AddComponent<dae::RenderComponent>()->SetTexture("background.png");
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
	auto livesUI = UIFactory::CreateUI_Lives({ 800, 600 }, "player.png");

	// Observer / Subject
	auto score = player->GetComponent<dae::ScoreComponent>();
	score->GetSubject().AddObserver(&m_pWinAchievement);

	auto lives = player->GetComponent<dae::LivesComponent>();
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());

	score->GetSubject().AddObserver(scoreUI->GetComponent<dae::ScoreDisplayComponent>());
	player->GetComponent<dae::ShootComponent>()->GetSubject().AddObserver(m_pBulletSpawner.get());

	// Enemies
	EnemyWave wave1{
		{{450,200},{350,250},{400,250},{450,250},{500,250},{550,250},{600,250},{500,200}}
	};

	m_pWaveSpawner->SpawnWave(wave1, score);

	// Add to scene
	m_pScene->Add(std::move(player));
	m_pScene->Add(std::move(livesUI));
	m_pScene->Add(std::move(scoreUI));
}

void dae::PlayState::OnExit()
{
	m_pScene->RemoveAll();
}

std::unique_ptr<dae::GameState> dae::PlayState::Update(float)
{
    if (m_playerDied) 
	{
        return std::make_unique<HighscoreState>();
    }

    return nullptr;
}
