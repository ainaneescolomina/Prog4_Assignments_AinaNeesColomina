#include "LevelLoader.h"
#include <fstream>
#include <string>
#include <iostream>

// Z = Bee
// G = Butterfly
// B = Boss Galaga
// . = Empty

std::vector<EnemySpawnData>LevelLoader::LoadLevel(const std::string& filepath)
{
    std::vector<EnemySpawnData> enemies{};

    std::ifstream inputFile(filepath);

    if (!inputFile.is_open())
    {
        std::cerr << "Could not open Level file.\n";
        return enemies;
    }

    constexpr float startX{ 150.f };
    constexpr float startY{ 100.f };

    constexpr float marginX{ 70.f };
    constexpr float marginY{ 60.f };

    std::string line{};
    int row{};

    while (std::getline(inputFile, line))
    {
        if (line.empty()) continue;
        for (int idx{}; idx < line.size(); ++idx)
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
                startX + idx * marginX,
                startY + row * marginY
            };

            enemies.push_back({type, pos});
        }

        ++row;
    }

    inputFile.close();

    return enemies;
}