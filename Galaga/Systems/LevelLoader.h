#pragma once

#include <vector>
#include <string>
#include "EnemyType.h"
#include <glm/glm.hpp>

// Data structure for storing raw configuration layouts parsed from file
struct EnemySpawnData
{
    EnemyType type;
    glm::vec2 pos;
    int row;
    float spawnDelay{};
};

// DESCRIPTION: Reads local level layout configuration text matrices line-by-line, calculating 
// starting coordinate grid offsets and staggered deployment timings for wave generation
class LevelLoader
{
public:
    static std::vector<EnemySpawnData>LoadLevel(const std::string& filepath);
};