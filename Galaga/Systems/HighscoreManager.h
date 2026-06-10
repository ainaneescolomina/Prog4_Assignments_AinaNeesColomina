#pragma once
#include <string>
#include <vector>

namespace dae
{
    struct HighscoreData
    {
        std::string name;
        int score;
    };

    class HighscoreManager final
    {
    public:
        HighscoreManager() = default;
        ~HighscoreManager() = default;

        std::vector<HighscoreData> LoadHighscores(const std::string& filepath);
        void SaveNewHighscore(const std::string& filepath, const std::string& name, int score);
    };
}