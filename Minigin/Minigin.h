#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <GameStateManager.h>

namespace dae
{
	class Minigin final
	{
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame(float delta_time);

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

		void SetInitialState(std::unique_ptr<GameState> pInitialState);
		GameStateManager& GetGameStateManager() { return m_gameStateManager; };

	private:
		GameStateManager m_gameStateManager;
		bool m_quit{};
	};
}