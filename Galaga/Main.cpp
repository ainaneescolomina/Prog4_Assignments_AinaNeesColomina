#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#pragma warning (pop)
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "InputManager.h"
//#include "Commands.h"
#include "Achievements.h"
#include "BulletSpawner.h"

#include "ServiceLocator.h"
#include "SdlSoundSystem.h"

// ---------------

//#include "Factory.h"
#include "WaveSpawner.h"

#include <filesystem>
namespace fs = std::filesystem;

static dae::WinOneGameAchievement g_WinAchievement;
static std::unique_ptr<dae::BulletSpawner> g_BulletSpawner;
static std::unique_ptr<WaveSpawner> g_WaveSpawner;
static void load()
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

	/*
	// --- Player 1 ---

	auto character1 = std::make_unique<dae::GameObject>();
	character1->AddComponent<dae::RenderComponent>()->SetTexture("player.png");
	character1->SetPosition(200.f, 200.f);

	character1->AddComponent<dae::TagComponent>("Player");
	character1->AddComponent<dae::ColliderComponent>(45.f, 45.f);
	
	// Bindings
	{
		float speed1 = 25.f;
		input.BindCommand(SDLK_W, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), 0.f, -speed1));

		input.BindCommand(SDLK_S, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), 0.f, speed1));

		input.BindCommand(SDLK_A, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), -speed1, 0.f));

		input.BindCommand(SDLK_D, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), speed1, 0.f));

		input.BindCommand(SDLK_C, dae::KeyState::Up,
			std::make_unique<dae::DamageCommand>(character1.get()));

		input.BindCommand(SDLK_Z, dae::KeyState::Up,
			std::make_unique<dae::ScoreCommand>(character1.get(), 10));

		input.BindCommand(SDLK_X, dae::KeyState::Up,
			std::make_unique<dae::ScoreCommand>(character1.get(), 100));
	}

	// --- UI ---
	{
		auto livesUI = std::make_unique<dae::GameObject>();
		auto livesUIComponent = livesUI->AddComponent<dae::LivesDisplayComponent>(3);
		livesUIComponent->SetTexture("player.png");
		livesUI->SetPosition(15, 500);

		auto scoreUI = std::make_unique<dae::GameObject>();
		auto scoreUIComponent = scoreUI->AddComponent<dae::ScoreDisplayComponent>(font);
		scoreUI->SetPosition(15, 450);

		auto lives = character1->AddComponent<dae::LivesComponent>(3);
		lives->GetSubject().AddObserver(livesUIComponent);
		character1->AddComponent<dae::ScoreComponent>()->GetSubject().AddObserver(scoreUIComponent);

		character1->GetComponent<dae::ColliderComponent>()->GetSubject().AddObserver(lives);

		scene.Add(std::move(livesUI));
		scene.Add(std::move(scoreUI));
	}

	// Achievement
	{
		character1->GetComponent<dae::ScoreComponent>()->GetSubject().AddObserver(&g_WinAchievement);
	}

	scene.Add(std::move(character1));

	// --- Player 2 ---
	auto character2 = std::make_unique<dae::GameObject>();
	character2->AddComponent<dae::RenderComponent>()->SetTexture("enemy_butterfly.png");
	character2->SetPosition(250.f, 200.f);

	character2->AddComponent<dae::TagComponent>("Enemy");
	character2->AddComponent<dae::ColliderComponent>(45.f, 45.f);

	// Bindings
	{
		float speed2 = 50.f;
		input.BindGamepadCommand(dae::GAMEPAD_DPAD_UP, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), 0.f, -speed2));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), 0.f, speed2));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), -speed2, 0.f));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), speed2, 0.f));

		input.BindGamepadCommand(dae::GAMEPAD_X, dae::KeyState::Up,
			std::make_unique<dae::DamageCommand>(character2.get()));

		input.BindGamepadCommand(dae::GAMEPAD_A, dae::KeyState::Up,
			std::make_unique<dae::ScoreCommand>(character2.get(), 10));

		input.BindGamepadCommand(dae::GAMEPAD_B, dae::KeyState::Up,
			std::make_unique<dae::ScoreCommand>(character2.get(), 100));
	}

	// UI
	{
		auto livesUI = std::make_unique<dae::GameObject>();
		auto livesUIComponent = livesUI->AddComponent<dae::LivesDisplayComponent>(3);
		livesUIComponent->SetTexture("enemy_butterfly.png");
		livesUI->SetPosition(800, 500);

		auto scoreUI = std::make_unique<dae::GameObject>();
		auto scoreUIComponent = scoreUI->AddComponent<dae::ScoreDisplayComponent>(font);
		scoreUI->SetPosition(775, 450);

		character2->AddComponent<dae::LivesComponent>(3)->GetSubject().AddObserver(livesUIComponent);
		character2->AddComponent<dae::ScoreComponent>()->GetSubject().AddObserver(scoreUIComponent);

		scene.Add(std::move(livesUI));
		scene.Add(std::move(scoreUI));
	}

	// Achievement
	{
		character2->GetComponent<dae::ScoreComponent>()->GetSubject().AddObserver(&g_WinAchievement);
	}

	scene.Add(std::move(character2));
	*/

	/////////////

	// Managers
	g_BulletSpawner = std::make_unique<dae::BulletSpawner>(scene);
	g_WaveSpawner = std::make_unique<WaveSpawner>(scene);

	// Player
	auto player = ActorFactory::CreatePlayer(input, { 450, 400 });

	// UI
	auto livesUI = UIFactory::CreateUI_Lives({ 15, 500 }, "player.png");
	auto livesScore = UIFactory::CreateUI_Score(font, { 15, 450 });

	// Observer / Subject
	auto score = player->GetComponent<dae::ScoreComponent>();
	score->GetSubject().AddObserver(&g_WinAchievement);

	auto lives = player->GetComponent<dae::LivesComponent>();
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	lives->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	
	score->GetSubject().AddObserver(livesScore->GetComponent<dae::ScoreDisplayComponent>());
	player->GetComponent<dae::ShootComponent>()->GetSubject().AddObserver(g_BulletSpawner.get());

	// Enemies
	EnemyWave wave1{
		{{400,150},{450,150},{500,150},{550,150}}
	};

	g_WaveSpawner->SpawnWave(wave1, score); 

	// Add to scene
	scene.Add(std::move(player));
	scene.Add(std::move(livesUI));
	scene.Add(std::move(livesScore));
}

int main(int, char*[]) {
#if USE_STEAMWORKS
	if (!SteamAPI_Init())
	{
		std::cerr << "SteamAPI_Init() failed! Achievements will not work." << std::endl;
	}
	else
	{
		std::cout << "SteamAPI_Init() Success!" << std::endl;
}
#endif

#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);

	dae::servicelocator::shutdown();

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif

    return 0;
}
