#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

namespace dae {
    class GameObject;
    class InputManager;
    class Font;
}

namespace ActorFactory
{
    std::unique_ptr<dae::GameObject> CreatePlayer(dae::InputManager& input, const glm::vec2& pos);
    std::unique_ptr<dae::GameObject> CreateEnemy(const glm::vec2& pos);
}

namespace UIFactory
{
    std::unique_ptr<dae::GameObject> CreateUI_Lives(const glm::vec2& pos, const std::string& texture);
    std::unique_ptr<dae::GameObject> CreateUI_Score(std::shared_ptr<dae::Font> font, const glm::vec2& pos);
    std::unique_ptr<dae::GameObject> CreateUI_Text(std::shared_ptr<dae::Font> font, const glm::vec2& pos, std::string text, const SDL_Color& color = SDL_Color{ 255, 255, 255, 255 });
}