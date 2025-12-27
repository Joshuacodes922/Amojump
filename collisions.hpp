#include "Scene.hpp"
#include <vector>

class ScoreManager;
struct collisionXY {
	bool collide;
	float aMinX, aMaxX, aMinY, aMaxY,bMinX,bMinY,bMaxX,bMaxY;
};

class CollisionHandler {

public:
	bool hasPlayerDied = false;
	ScoreManager& scoreManager;
	CollisionHandler(ScoreManager& scoreManager):scoreManager(scoreManager) {
		hasPlayerDied = false;
	}

	void resolveCollisions(std::vector<SceneObject>& sceneObjects);
	collisionXY checkAABBCollisions(SceneObject& obj1, SceneObject& obj2);
};