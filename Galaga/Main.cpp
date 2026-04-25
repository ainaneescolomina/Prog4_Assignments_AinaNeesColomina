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
#include "SpawnSystem.h"

// ---------------

//#include "Factory.h"
#include "WaveSpawner.h"

#include <filesystem>
namespace fs = std::filesystem;

static dae::WinOneGameAchievement g_WinAchievement;
static std::unique_ptr<dae::BulletSpawner> g_SpawManager;
static std::unique_ptr<WaveSpawner> g_WaveSpawner;
static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	/*
	auto objBackground = std::make_unique<dae::GameObject>();
	objBackground->AddComponent<dae::RenderComponent>();
	objBackground->GetComponent<dae::RenderComponent>()->SetTexture("background.png");
	
	scene.Add(std::move(objBackground));

	auto objLogo = std::make_unique<dae::GameObject>();
	objLogo->AddComponent<dae::RenderComponent>();
	objLogo->GetComponent<dae::RenderComponent>()->SetTexture("logo.png");
	objLogo->SetPosition(358, 180);

	scene.Add(std::move(objLogo));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto objTitle = std::make_unique<dae::GameObject>();
	objTitle->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	objTitle->GetComponent<dae::TextComponent>()->SetColor({ 255, 255, 0, 255 });
	objTitle->SetPosition(292, 20);

	scene.Add(std::move(objTitle));

	auto objFps = std::make_unique<dae::GameObject>();
	objFps->AddComponent<dae::TextComponent>("0 FPS", font);
	objFps->AddComponent<dae::FPSComponent>();

	scene.Add(std::move(objFps));
	*/

	///////////////
	auto font = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 36);
	auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Silkscreen-Regular.ttf", 18);
	auto& input = dae::InputManager::GetInstance();

	// --- UI ---
	auto instructionsPlayer1 = std::make_unique<dae::GameObject>();
	instructionsPlayer1->AddComponent<dae::TextComponent>("Move Player 1 with WASD, use C to self inflict damage, use Z and X to get points", fontSmall);
	instructionsPlayer1->SetPosition(15.f, 15.f);

	scene.Add(std::move(instructionsPlayer1));

	auto instructionsPlayer2 = std::make_unique<dae::GameObject>();
	instructionsPlayer2->AddComponent<dae::TextComponent>("Move Player 2 with D-Pad, use X to self inflict damage, use A and B to get points", fontSmall);
	instructionsPlayer2->SetPosition(15.f, 40.f);

	scene.Add(std::move(instructionsPlayer2));

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
	g_SpawManager = std::make_unique<dae::BulletSpawner>(scene);
	g_WaveSpawner = std::make_unique<WaveSpawner>(scene);

	// Player
	auto player = ActorFactory::CreatePlayer(input, { 200, 200 });

	// UI
	auto livesUI = UIFactory::CreateUI_Lives({ 15, 500 }, "player.png");
	auto livesScore = UIFactory::CreateUI_Score(font, { 15, 450 });

	// Observer / Subject
	player->GetComponent<dae::LivesComponent>()->GetSubject().AddObserver(livesUI->GetComponent<dae::LivesDisplayComponent>());
	player->GetComponent<dae::ScoreComponent>()->GetSubject().AddObserver(livesScore->GetComponent<dae::ScoreDisplayComponent>());
	player->GetComponent<dae::ShootComponent>()->GetSubject().AddObserver(g_SpawManager.get());

	// Add to scene
	scene.Add(std::move(player));
	scene.Add(std::move(livesUI));
	scene.Add(std::move(livesScore));

	// Enemies
	EnemyWave wave1{
		{{100,100},{150,100},{200,100},{250,100}}
	};

	g_WaveSpawner->SpawnWave(wave1); 
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

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif

    return 0;
}
