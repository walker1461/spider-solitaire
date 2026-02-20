#include "persistence.h"
#include <fstream>
#include <ios>
#include <iostream>

void saveScores(const std::vector<int>& scores) {
    std::ofstream file("assets/scores.txt", std::ios::trunc);
    for (const int s : scores) {
        file << s << "\n";
    }
}

std::vector<int> loadScores() {
    std::vector<int> scores;
    std::ifstream file("assets/scores.txt");

    int score;
    while (file >> score) {
        scores.push_back(score);
    }
    std::cout << std::filesystem::current_path() << "\n";
    std::cout << "Loaded " << scores.size() << " scores\n";
    return scores;
}