
class ScoreManager {
public:
	float score;
	float highScore;

	ScoreManager() {
		score = 0;
		initHighscore();
	}
	void incrementScore(float amount) {
		score += amount;
	}

	void initHighscore();

	void setHighScore(float newScore);
};