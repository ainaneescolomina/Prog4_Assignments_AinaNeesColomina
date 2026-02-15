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

	//auto go = std::make_unique<dae::GameObject>();
	//go->SetTexture("background.png");
	//scene.Add(std::move(go));

	//go = std::make_unique<dae::GameObject>();
	//go->SetTexture("logo.png");
	//go->SetPosition(358, 180);
	//scene.Add(std::move(go));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	//auto to = std::make_unique<dae::TextObject>("Programming 4 Assignment", font);
	//to->SetColor({ 255, 255, 0, 255 });
	//to->SetPosition(292, 20);
	//scene.Add(std::move(to));

	auto obj = std::make_unique<dae::GameObject>();

	auto text = obj->AddComponent<dae::TextComponent>("FPS: 0", font);
	auto fps = obj->AddComponent<dae::FPSComponent>();

	if (text)
	{
		text->SetText("FPS: " + std::to_string(fps->GetFPS()));
	}


	// Pass raw pointer to the GameObject, not &obj
	//auto fps = std::make_unique<dae::FPSComponent>(obj.get());
	//auto txt = std::make_unique<dae::TextComponent>(obj.get(), std::to_string(fps->GetFPS()), font);

	// Add components to obj
	//obj->AddComponent(std::move(txt));
	//obj->AddComponent(std::move(fps));

	scene.Add(std::move(obj));


	//auto to = std::make_unique<dae::TextObject>("Programming 4 Assignment", font);
	//to->SetColor({ 255, 255, 0, 255 });
	//to->SetPosition(292, 20);
	//scene.Add(std::move(to));
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
