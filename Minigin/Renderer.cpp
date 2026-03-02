#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Renderer.h"
#include "SceneManager.h"
#include "Texture2D.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <array>
#include <chrono>

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	m_renderer = SDL_CreateRenderer(window, nullptr);
	if (m_renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#if __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void dae::Renderer::Render() const
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(); // For demonstration purposes, do not keep this in your engine

	// ----- Exercice 1 -----
	{
		bool exercice1_active = true;
        static int value = 100;

		ImGui::Begin("Exercise 1", &exercice1_active, ImGuiWindowFlags_MenuBar);

		ImGui::Text("Title");
		ImGui::Separator();

		ImGui::SetNextItemWidth(80);
		ImGui::InputInt("##valueInput", &value);

		ImGui::SameLine();
		ImGui::Text("# samples");

		ImGui::Spacing();

        static std::array<int, 10000000>* arr = nullptr;
        static std::vector<float> data_exercice1;
        static bool hasResults = false;

		if (ImGui::Button("Thrash the cache"))
		{
            if (value == 0) return;
			if (arr == nullptr) arr = new std::array<int, 10000000>;

			data_exercice1.clear();

			for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
			{
				long long avgTime = 0;

				for (int i = 0; i < value; i++)
				{
					const auto start = std::chrono::high_resolution_clock::now();

					for (size_t j = 0; j < arr->size(); j += stepsize)
					{
						(*arr)[static_cast<int>(j)] *= 2;
					}

					const auto end = std::chrono::high_resolution_clock::now();

					avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				}

				avgTime /= value;

				data_exercice1.push_back((float)avgTime);
			}

			hasResults = true;

			delete arr;
			arr = nullptr;
		}

		if (hasResults && !data_exercice1.empty())
		{
			float maxValue = *std::max_element(data_exercice1.begin(), data_exercice1.end());

			ImGui::PlotLines(
				"##Exercice1",
				data_exercice1.data(),
				static_cast<int>(data_exercice1.size()),
				0,
				nullptr,
				0.0f,
				maxValue * 1.1f,
				ImVec2(400, 200)
			);
		}

		ImGui::End();
	}

    // ----- Exercise 2 -----
    {
        bool exercise2_active = true;
        static int value = 100;
        ImGui::Begin("Exercise 2", &exercise2_active);

        ImGui::Text("Title");
        ImGui::Separator();

        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("##valueInput", &value);

        ImGui::SameLine();
        ImGui::Text("# samples");

        ImGui::Spacing();

        static std::vector<float> dataValue_exercice2;
        static std::vector<float> dataPointer_exercice2;

        static bool hasValueResults = false;
        static bool hasPointerResults = false;
        float maxValue = 0.0f;

        struct TransformGameObject3D
        {
            float matrix[16] = {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1 };
        };

        // -------- VALUE VERSION --------
        if (ImGui::Button("Thrash the cache with GameObject3D"))
        {
            if (value == 0) return;
            struct GameObject3D
            {
                TransformGameObject3D transform;
                int ID;
            };

            auto arr = new std::array<GameObject3D, 10000000>;

            dataValue_exercice2.clear();

            for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
            {
                long long avgTime = 0;

                for (int i = 0; i < value; i++)
                {
                    const auto start = std::chrono::high_resolution_clock::now();

                    for (size_t j = 0; j < arr->size(); j += stepsize)
                    {
                        (*arr)[static_cast<int>(j)].ID *= 2;
                    }

                    const auto end = std::chrono::high_resolution_clock::now();
                    avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                }

                avgTime /= value;

                dataValue_exercice2.push_back((float)avgTime);
            }

            delete arr;
            hasValueResults = true;
        }

        // -------- GRAPH 1 --------
        if (hasValueResults)
            maxValue = std::max(maxValue, *std::max_element(dataValue_exercice2.begin(), dataValue_exercice2.end()));
        if (hasValueResults)
        {
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));

            ImGui::PlotLines(
                "##Exercice2",
                dataValue_exercice2.data(),
                (int)dataValue_exercice2.size(),
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
            if (value == 0) return;

            struct GameObject3DAlt
            {
                TransformGameObject3D* transform;
                int ID;
            };

            auto arr = new std::array<GameObject3DAlt, 10000000>;

            dataPointer_exercice2.clear();

            for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
            {
                long long avgTime = 0;

                for (int i = 0; i < value; i++)
                {
                    const auto start = std::chrono::high_resolution_clock::now();

                    for (size_t j = 0; j < arr->size(); j += stepsize)
                    {
                        (*arr)[static_cast<int>(j)].ID *= 2;
                    }

                    const auto end = std::chrono::high_resolution_clock::now();
                    avgTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                }

                avgTime /= value;

                dataPointer_exercice2.push_back((float)avgTime);
            }

            delete arr;
            hasPointerResults = true;
        }

        ImGui::Separator();

        // -------- GRAPH 2 --------
        if (hasPointerResults)
            maxValue = std::max(maxValue, *std::max_element(dataPointer_exercice2.begin(), dataPointer_exercice2.end()));

        if (hasPointerResults)
        {
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));

            ImGui::PlotLines(
                "##Exercice2",
                dataPointer_exercice2.data(),
                (int)dataPointer_exercice2.size(),
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
        if (hasValueResults && hasPointerResults)
        {
            ImGui::Text("Overlay Comparison");

            float maxValuePlot = *std::max_element(dataValue_exercice2.begin(), dataValue_exercice2.end());
            float maxPointerPlot = *std::max_element(dataPointer_exercice2.begin(), dataPointer_exercice2.end());
            float globalMax = std::max(maxValuePlot, maxPointerPlot);

            ImVec2 size(400, 200);

            // VALUE VERSION
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PlotLines("##Exercice2 - Value",
                dataValue_exercice2.data(),
                (int)dataValue_exercice2.size(),
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
                dataPointer_exercice2.data(),
                (int)dataPointer_exercice2.size(),
                0,
                nullptr,
                0.0f,
                globalMax * 1.1f,
                size);
            ImGui::PopStyleColor(2);
        }

        ImGui::End();
    }

	ImGui::Render();

	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
	SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }
