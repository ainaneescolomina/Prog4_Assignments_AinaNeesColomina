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

#include "ServiceLocator.h"
#include "States/MenuState.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

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

	auto menuState = std::make_unique<dae::MenuState>(&engine.GetGameStateManager());
	engine.SetInitialState(std::move(menuState));

	engine.Run([]() {});

	dae::servicelocator::shutdown();

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif

    return 0;
}
