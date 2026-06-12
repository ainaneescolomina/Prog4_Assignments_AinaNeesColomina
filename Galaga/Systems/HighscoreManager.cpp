#include "HighscoreManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

std::vector<dae::HighscoreData> dae::HighscoreManager::LoadHighscores(const std::string& filepath)
{
    std::vector<dae::HighscoreData> scores{};
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Could not open Highscore file.\n";
        return scores;
    }

    std::string name{};
    int score{};

    while (file >> name >> score)
    {
        scores.push_back({ name, score });
    }

    file.close();

    return scores;
}

void dae::HighscoreManager::SaveNewHighscore(const std::string& filepath, const std::string& name, int score)
{
    std::vector<dae::HighscoreData> currentScores = LoadHighscores(filepath);
    currentScores.push_back({ name, score });

    // sort by score amount using lambda
    std::sort(currentScores.begin(), currentScores.end(), [](const dae::HighscoreData& a, const dae::HighscoreData& b) {
        return a.score > b.score; // Higher score comes first
        });

    // only save the best five scores
    if (currentScores.size() > 5)
        currentScores.resize(5);

    std::ofstream outFile(filepath, std::ios::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Error writing to Highscore file.\n";
        return;
    }

    for (const auto& entry : currentScores)
    {
        outFile << entry.name << " " << entry.score << "\n";
    }

    outFile.close();
}

int dae::HighscoreManager::GetBestHighscore(const std::string& filepath)
{
    std::vector<dae::HighscoreData> scores = LoadHighscores(filepath);
    
    if (scores.empty()) return 0;
    return scores[0].score;
}
