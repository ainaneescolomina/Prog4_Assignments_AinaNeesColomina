#pragma once

#include <vector>
#include <string>
#include "EnemyType.h"
#include <glm/glm.hpp>

// Data structures for the enemy data
struct EnemySpawnData
{
    EnemyType type;
    glm::vec2 pos;
    float spawnDelay{};
};

class LevelLoader
{
public:
    static std::vector<EnemySpawnData>LoadLevel(const std::string& filepath);
};