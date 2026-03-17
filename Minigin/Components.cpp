#include "Components.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/gtc/constants.hpp>
#include "Renderer.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <array>
#include <vector>
#include <chrono>

void dae::RenderComponent::Render() const
{
	if (m_texture == nullptr) return;
	const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextComponent::Update(float)
{
	if (m_needsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);
		m_texture = std::make_shared<Texture2D>(texture);
		m_needsUpdate = false;
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_color = color;
	m_needsUpdate = true;
}

dae::FPSComponent::FPSComponent(GameObject* ownerRef)
	:Component(ownerRef)
{
	m_pTextComponent = GetOwner()->GetComponent<TextComponent>();
}

void dae::FPSComponent::Update(float delta_time)
{
	m_elapsedTime += delta_time;
	m_frameCount++;

	if (m_elapsedTime >= 1.0f)
	{
		auto newFps = static_cast<float>(m_frameCount) / m_elapsedTime;

		if (std::fabs(m_fps - newFps) >= 0.01f)
		{
			m_fps = newFps;
			if (m_pTextComponent)
			{
				std::ostringstream ss;
				ss << std::fixed << std::setprecision(2) << m_fps;
				m_pTextComponent->SetText(ss.str() + " FPS");
			}
		}

		// Reset counters
		m_elapsedTime = 0.0f;
		m_frameCount = 0;
	}
}

float dae::FPSComponent::GetFPS() const
{
	return m_fps;
}

dae::RotatorComponent::RotatorComponent(GameObject* ownerRef, float radius, float speed)
	: Component(ownerRef),
	m_radius(radius),
	m_speed(speed)
{
	m_startPosition = GetOwner()->GetTransform().GetPosition();
}

void dae::RotatorComponent::Update(float deltaTime)
{
    m_angle += m_speed * deltaTime;
    m_angle = std::fmod(m_angle, glm::two_pi<float>());

    float offsetX = cos(m_angle) * m_radius;
    float offsetY = sin(m_angle) * m_radius;

    glm::vec3 center{};

    if (auto parent = GetOwner()->GetParent())
        center = parent->GetTransform().GetWorldPosition();
    else
        center = GetOwner()->GetTransform().GetWorldPosition();

    GetOwner()->SetPosition(center.x + offsetX,
                            center.y + offsetY);
}

/*
void dae::Exercise1Component::Render() const
{
    bool active = true;

    ImGui::Begin("Exercise 1", &active, ImGuiWindowFlags_MenuBar);

    ImGui::Text("Title");
    ImGui::Separator();

    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##valueInput", &m_value);

    ImGui::SameLine();
    ImGui::Text("# samples");

    ImGui::Spacing();

    if (ImGui::Button("Thrash the cache"))
    {
        if (m_value == 0) return;

        auto arr = new std::array<int, 10000000>;
        m_data.clear();

        for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
        {
            long long avgTime = 0;

            for (int i = 0; i < m_value; i++)
            {
                const auto start = std::chrono::high_resolution_clock::now();

                for (size_t j = 0; j < arr->size(); j += stepsize)
                {
                    (*arr)[static_cast<int>(j)] *= 2;
                }

                const auto end = std::chrono::high_resolution_clock::now();

                avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }

            avgTime /= m_value;

            m_data.push_back((float)avgTime);
        }

        delete arr;
        m_hasResults = true;
    }

    if (m_hasResults && !m_data.empty())
    {
        float maxValue = *std::max_element(m_data.begin(), m_data.end());

        ImGui::PlotLines(
            "##Exercise1",
            m_data.data(),
            (int)m_data.size(),
            0,
            nullptr,
            0.0f,
            maxValue * 1.1f,
            ImVec2(400, 200)
        );
    }

    ImGui::End();
}

void dae::Exercise2Component::Render() const
{
    bool active = true;

    ImGui::Begin("Exercise 2", &active);

    ImGui::Text("Title");
    ImGui::Separator();

    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##valueInput", &m_value);

    ImGui::SameLine();
    ImGui::Text("# samples");

    ImGui::Spacing();

    struct TransformGameObject3D
    {
        float matrix[16]{
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1 };
    };

    float maxValue = 0.f;
    // -------- VALUE VERSION --------
    if (ImGui::Button("Thrash the cache with GameObject3D"))
    {
        if (m_value == 0) return;

        struct GameObject3D
        {
            TransformGameObject3D transform;
            int ID;
        };

        auto arr = new std::array<GameObject3D, 10000000>;

        m_valueData.clear();

        for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
        {
            long long avgTime = 0;

            for (int i = 0; i < m_value; i++)
            {
                const auto start = std::chrono::high_resolution_clock::now();

                for (size_t j = 0; j < arr->size(); j += stepsize)
                {
                    (*arr)[static_cast<int>(j)].ID *= 2;
                }

                const auto end = std::chrono::high_resolution_clock::now();

                avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }

            avgTime /= m_value;

            m_valueData.push_back((float)avgTime);
        }

        delete arr;
        m_hasValueResults = true;
    }

    // -------- GRAPH 1 --------
    if (m_hasValueResults)
        maxValue = std::max(maxValue, *std::max_element(m_valueData.begin(), m_valueData.end()));
    if (m_hasValueResults)
    {
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));

        ImGui::PlotLines(
            "##Exercice2",
            m_valueData.data(),
            (int)m_valueData.size(),
            0,
            nullptr,
            0.0f,
            maxValue * 1.1f,
            ImVec2(400, 200)
        );

        ImGui::PopStyleColor();
    }

    ImGui::Spacing();

    // -------- POINTER VERSION --------
    if (ImGui::Button("Thrash the cache with GameObject3DAlt"))
    {
        if (m_value == 0) return;

        struct GameObject3DAlt
        {
            TransformGameObject3D* transform;
            int ID;
        };

        auto arr = new std::array<GameObject3DAlt, 10000000>;

        m_pointerData.clear();

        for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
        {
            long long avgTime = 0;

            for (int i = 0; i < m_value; i++)
            {
                const auto start = std::chrono::high_resolution_clock::now();

                for (size_t j = 0; j < arr->size(); j += stepsize)
                {
                    (*arr)[static_cast<int>(j)].ID *= 2;
                }

                const auto end = std::chrono::high_resolution_clock::now();

                avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }

            avgTime /= m_value;

            m_pointerData.push_back((float)avgTime);
        }

        delete arr;
        m_hasPointerResults = true;
    }

    ImGui::Separator();

    // -------- GRAPH 2 --------
    if (m_hasPointerResults)
        maxValue = std::max(maxValue, *std::max_element(m_pointerData.begin(), m_pointerData.end()));

    if (m_hasPointerResults)
    {
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));

        ImGui::PlotLines(
            "##Exercice2",
            m_pointerData.data(),
            (int)m_pointerData.size(),
            0,
            nullptr,
            0.0f,
            maxValue * 1.1f,
            ImVec2(400, 200)
        );

        ImGui::PopStyleColor();
    }

    ImGui::Separator();

    // -------- GRAPH 3 (Overlay Combined) --------
    if (m_hasValueResults && m_hasPointerResults)
    {
        ImGui::Text("Overlay Comparison");

        float maxValuePlot = *std::max_element(m_valueData.begin(), m_valueData.end());
        float maxPointerPlot = *std::max_element(m_valueData.begin(), m_valueData.end());
        float globalMax = std::max(maxValuePlot, maxPointerPlot);

        ImVec2 size(400, 200);

        // VALUE VERSION
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
        ImGui::PlotLines("##Exercice2 - Value",
            m_valueData.data(),
            (int)m_valueData.size(),
            0,
            nullptr,
            0.0f,
            globalMax * 1.1f,
            size);
        ImGui::PopStyleColor();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - size.y - 4);

        // POINTER VERSION
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::PlotLines("##Exercice2 - Pointer",
            m_pointerData.data(),
            (int)m_pointerData.size(),
            0,
            nullptr,
            0.0f,
            globalMax * 1.1f,
            size);
        ImGui::PopStyleColor(2);
    }

    ImGui::End();
}
*/

void dae::LivesComponent::TakeDamage(int damage)
{
    m_lives -= damage;

    if (m_lives <= 0)
    {
        Event e(make_sdbm_hash("PlayerDied"));
        NotifyObservers(e);
    }

    Event e(make_sdbm_hash("UpdateLives"));
    NotifyObservers(e);
}

void dae::ScoreComponent::AddScore(int score)
{
    m_score += score;

    Event e(make_sdbm_hash("UpdateScore"));
    NotifyObservers(e);
}

void dae::LivesDisplayComponent::Notify(Event event, Subject* sender)
{
    if (event.id == make_sdbm_hash("UpdateLives"))
    {
        LivesComponent* component = static_cast<LivesComponent*>(sender);
        m_lives = component->GetLives();
    }
}

void dae::LivesDisplayComponent::Render() const
{
    if (m_texture == nullptr) return;
    const auto& pos = GetOwner()->GetTransform().GetWorldPosition();
    const float spacing = 65.f;

    for (int i = 0; i < m_lives; ++i)
    {
        Renderer::GetInstance().RenderTexture(*m_texture, pos.x + i * spacing, pos.y);
    }
}

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* ownerRef, std::shared_ptr<Font> font, const SDL_Color& color)
    : TextComponent(ownerRef, "", font, color)
{
    UpdateText();
}

void dae::ScoreDisplayComponent::Notify(Event event, Subject* sender)
{
    if (event.id == make_sdbm_hash("UpdateScore"))
    {
        ScoreComponent* component = static_cast<ScoreComponent*>(sender);
        m_score = component->GetScore();

        UpdateText();
    }
}

void dae::ScoreDisplayComponent::UpdateText()
{
    std::string text = "Score: " + std::to_string(m_score);
    SetText(text);
}
