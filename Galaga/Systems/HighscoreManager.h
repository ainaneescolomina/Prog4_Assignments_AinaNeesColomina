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

    // DESCRIPTION: Manages reading and saving data to local files to keep leaderboard history 
    // across games, sorting data, and clamping entries to top-tier caps.
    class HighscoreManager final
    {
    public:
        HighscoreManager() = default;
        ~HighscoreManager() = default;

        // Streams local file records directly into memory arrays
        std::vector<HighscoreData> LoadHighscores(const std::string& filepath);
        void SaveNewHighscore(const std::string& filepath, const std::string& name, int score);
        int GetBestHighscore(const std::string& filepath);
    };
}