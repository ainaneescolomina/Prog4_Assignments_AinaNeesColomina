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

//#include "Factory.h"

void PlayState::OnEnter()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto soundSystem = std::make_unique<dae::sdl_sound_system>();
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

	scene.Add(std::move(background));

	auto movementInstructions = std::make_unique<dae::GameObject>();
	movementInstructions->AddComponent<dae::TextComponent>("Move Player with WASD or D-Pad, shoot bullets with E or button A", fontSmall);
	movementInstructions->SetPosition(15.f, 15.f);

	scene.Add(std::move(movementInstructions));

	auto liveInstructions = std::make_unique<dae::GameObject>();
	liveInstructions->AddComponent<dae::TextComponent>("You lose lives by colliding against enemies", fontSmall);
	liveInstructions->SetPosition(15.f, 40.f);

	scene.Add(std::move(liveInstructions));

	auto scoreInstructions = std::make_unique<dae::GameObject>();
	scoreInstructions->AddComponent<dae::TextComponent>("You gain points by shooting enemies", fontSmall);
	scoreInstructions->SetPosition(15.f, 65.f);

	scene.Add(std::move(scoreInstructions));

	// Managers
	m_pBulletSpawner = std::make_unique<dae::BulletSpawner>(scene);
	m_pWaveSpawner = std::make_unique<WaveSpawner>(scene);

	// Player
	auto player = ActorFactory::CreatePlayer(input, { 450, 400 });

	// UI
	auto livesUI = UIFactory::CreateUI_Lives({ 15, 500 }, "player.png");
	auto livesScore = UIFactory::CreateUI_Score(font, { 15, 450 });

	// Observer / Subject
	auto score = player->GetComponent<dae::ScoreComponent>();
	score->GetSubject().AddObserver(&m_pWinAchievement);

	auto lives = player->GetComponent<dae::LivesComponent>();
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());

	score->GetSubject().AddObserver(livesScore->GetComponent<dae::ScoreDisplayComponent>());
	player->GetComponent<dae::ShootComponent>()->GetSubject().AddObserver(m_pBulletSpawner.get());

	// Enemies
	EnemyWave wave1{
		{{400,150},{450,150},{500,150},{550,150}}
	};

	m_pWaveSpawner->SpawnWave(wave1, score);

	// Add to scene
	scene.Add(std::move(player));
	scene.Add(std::move(livesUI));
	scene.Add(std::move(livesScore));
}

std::unique_ptr<dae::GameState> PlayState::Update(float)
{
    if (m_playerDied) {
        return std::make_unique<HighscoreState>();
    }
    return nullptr;
}
