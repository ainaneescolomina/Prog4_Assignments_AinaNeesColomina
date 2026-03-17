#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include "InputManager.h"
#include "Commands.h"

#include <filesystem>
namespace fs = std::filesystem;

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
	auto& input = dae::InputManager::GetInstance();

	// --- Player 1 ---
	auto character1 = std::make_unique<dae::GameObject>();
	character1->AddComponent<dae::RenderComponent>()->SetTexture("player.png");
	character1->SetPosition(200.f, 200.f);
	
	// Bindings
	{
		float speed1 = 5.f;
		input.BindCommand(SDLK_W, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), 0.f, -speed1));

		input.BindCommand(SDLK_S, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), 0.f, speed1));

		input.BindCommand(SDLK_A, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), -speed1, 0.f));

		input.BindCommand(SDLK_D, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character1.get(), speed1, 0.f));

		input.BindCommand(SDLK_M, dae::KeyState::Up,
			std::make_unique<dae::ShootCommand>(character1.get(), speed1, 0.f));
	}

	// UI
	{
		auto livesUI = std::make_unique<dae::GameObject>();
		auto livesUIComponent = livesUI->AddComponent<dae::LivesDisplayComponent>(3);
		livesUIComponent->SetTexture("player.png");
		livesUI->SetPosition(15, 500);

		auto scoreUI = std::make_unique<dae::GameObject>();
		auto scoreUIComponent = scoreUI->AddComponent<dae::ScoreDisplayComponent>(font);
		scoreUI->SetPosition(15, 450);

		character1->AddComponent<dae::LivesComponent>(3)->AddObserver(livesUIComponent);
		character1->AddComponent<dae::ScoreComponent>()->AddObserver(scoreUIComponent);

		scene.Add(std::move(livesUI));
		scene.Add(std::move(scoreUI));
	}

	scene.Add(std::move(character1));

	// Player 2
	auto character2 = std::make_unique<dae::GameObject>();
	character2->AddComponent<dae::RenderComponent>()->SetTexture("enemy_butterfly.png");
	character2->SetPosition(250.f, 200.f);

	// Bindings
	{
		float speed2 = 10.f;
		input.BindGamepadCommand(dae::GAMEPAD_DPAD_UP, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), 0.f, -speed2));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_DOWN, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), 0.f, speed2));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_LEFT, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), -speed2, 0.f));

		input.BindGamepadCommand(dae::GAMEPAD_DPAD_RIGHT, dae::KeyState::Pressed,
			std::make_unique<dae::MoveCommand>(character2.get(), speed2, 0.f));
	}

	// UI
	{
		auto livesUI = std::make_unique<dae::GameObject>();
		auto livesUIComponent = livesUI->AddComponent<dae::LivesDisplayComponent>(3);
		livesUIComponent->SetTexture("enemy_butterfly.png");
		livesUI->SetPosition(835, 500);

		auto scoreUI = std::make_unique<dae::GameObject>();
		auto scoreUIComponent = scoreUI->AddComponent<dae::ScoreDisplayComponent>(font);
		scoreUI->SetPosition(820, 450);

		character2->AddComponent<dae::LivesComponent>(3)->AddObserver(livesUIComponent);
		character2->AddComponent<dae::ScoreComponent>()->AddObserver(scoreUIComponent);

		scene.Add(std::move(livesUI));
		scene.Add(std::move(scoreUI));
	}

	scene.Add(std::move(character2));

	/////////////
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
