#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

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
