#include "scoreManager.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;

void ScoreManager::initHighscore()
{
	
	std::ifstream file("highscore.json");

	if (!file.is_open()) {
		return;
	}

	json data = json::parse(file);

	highScore = data["highscore"];
}

void ScoreManager::setHighScore(float newScore)
{
	highScore = newScore;
	std::ofstream file("highscore.json");

	json data;
	data["highscore"] = newScore;

	file << std::setw(4) << data << std::endl;
}
