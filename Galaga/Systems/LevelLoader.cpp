#include "LevelLoader.h"
#include <fstream>
#include <string>
#include <iostream>

// Mapping Legend:
// Z = Bee (Zako)
// G = Butterfly (Goei)
// B = Boss Galaga
// . = Empty space cell

std::vector<EnemySpawnData>LevelLoader::LoadLevel(const std::string& filepath)
{
    std::vector<EnemySpawnData> enemies{};

    std::ifstream inputFile(filepath);

    if (!inputFile.is_open())
    {
        std::cerr << "Could not open Level file.\n";
        return enemies;
    }

    constexpr float startX{ 625.f };
    constexpr float startY{ 300.f };

    constexpr float marginX{ 70.f };
    constexpr float marginY{ 60.f };

    std::string line{};
    int row{};
    float delay = 0.f; // Aggregates delay for each enemy to achive a delayed start

    // Process file line-by-line where each string represents an independent horizontal grid row
    while (std::getline(inputFile, line))
    {
        if (line.empty()) continue;
        for (int idx{}; idx < static_cast<int>(line.size()); ++idx)
        {
            EnemyType type{};

            switch (line[idx])
            {
            case 'Z':
                type = EnemyType::Bee;
                break;

            case 'G':
                type = EnemyType::Butterfly;
                break;

            case 'B':
                type = EnemyType::BossGalaga;
                break;

            case '.':
            default:
                continue;
            }

            glm::vec2 pos
            {
                startX - idx * marginX,
                startY - row * marginY
            };

            enemies.push_back({type, pos, row, delay});

            delay += 0.2f;
        }

        delay += 3.f;
        ++row;
    }

    inputFile.close();

    return enemies;
}