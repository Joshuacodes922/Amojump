#include "collisions.hpp"
#include "scoreManager.hpp"

void CollisionHandler::resolveCollisions(std::vector<SceneObject>& sceneObjects) {
    for (auto& obj : sceneObjects) {
        if (obj.gravityAffected) obj.isGrounded = false;
    }

    for (size_t i = 0; i < sceneObjects.size(); i++) {
        for (size_t j = i + 1; j < sceneObjects.size(); j++) {
            SceneObject& a = sceneObjects[i];
            SceneObject& b = sceneObjects[j];

            collisionXY collision = checkAABBCollisions(a, b);
            if (collision.collide) {
                // Calculate overlaps
                float xOverlap = std::min(collision.aMaxX, collision.bMaxX) - std::max(collision.aMinX, collision.bMinX);
                float yOverlap = std::min(collision.aMaxY, collision.bMaxY) - std::max(collision.aMinY, collision.bMinY);

                // Resolve the smaller overlap to prevent "popping" through floors
                if (xOverlap < yOverlap) {
                    // Resolve X
                    if (a.gravityAffected && !b.gravityAffected) {
                        a.position.x += (a.position.x < b.position.x) ? -xOverlap : xOverlap;
                        a.velocity.x = 0;
                    }
                    else if (b.gravityAffected && !a.gravityAffected) {
                        b.position.x += (b.position.x < a.position.x) ? -xOverlap : xOverlap;
                        b.velocity.x = 0;
                    }
                }
                else {
                    // Resolve Y
                    if (a.gravityAffected && !b.gravityAffected) {
                        if (a.position.y < b.position.y) { // A is below B (hitting head or grounded depending on Y direction)
                            a.position.y -= yOverlap;
                            a.isGrounded = true;
                        }
                        else {
                            a.position.y += yOverlap;
                        }
                        a.velocity.y = 0;
                    }
                    else if (b.gravityAffected && !a.gravityAffected) {
                        if (b.position.y < a.position.y) {
                            b.position.y -= yOverlap;
                            b.isGrounded = true;
                        }
                        else {
                            b.position.y += yOverlap;
                        }
                        b.velocity.y = 0;
                    }
                }
            }
        }
    }
}

collisionXY CollisionHandler::checkAABBCollisions(SceneObject& a, SceneObject& b)
{
	float AshrinkFactorX = 1;
	float AshrinkFactorY = 1;
	float BshrinkFactorX = 1;
	float BshrinkFactorY = 1;

	if (b.gameObjectType == GameObjectType::END) {
		BshrinkFactorY = 0.1;
	}

	if (a.gameObjectType == GameObjectType::END) {
		AshrinkFactorY = 0.1;
	}

    if (a.gameObjectType == GameObjectType::CHARACTER) {
        AshrinkFactorY = 0.1;
        AshrinkFactorX = 0.4;
    }

    if (b.gameObjectType == GameObjectType::CHARACTER) {
        BshrinkFactorY = 0.1;
        AshrinkFactorX = 0.4;
    }

	// 3. Calculate Bounds for A (using factors)
	float aMinX = a.position.x - (a.scale.x * AshrinkFactorX) / 2.0f;
	float aMaxX = a.position.x + (a.scale.x * AshrinkFactorX) / 2.0f;
	float aMinY = a.position.y - (a.scale.y * AshrinkFactorY) / 2.0f;
	float aMaxY = a.position.y + (a.scale.y * AshrinkFactorY) / 2.0f;

	// 4. Calculate Bounds for B (using factors)
	float bMinX = b.position.x - (b.scale.x * BshrinkFactorX) / 2.0f;
	float bMaxX = b.position.x + (b.scale.x * BshrinkFactorX) / 2.0f;
	float bMinY = b.position.y - (b.scale.y * BshrinkFactorY) / 2.0f;
	float bMaxY = b.position.y + (b.scale.y * BshrinkFactorY) / 2.0f;

	bool res = (aMinX <= bMaxX && bMinX <= aMaxX) && (aMinY <= bMaxY && bMinY <= aMaxY);

	bool isPlayerEndCollision =
		(a.gameObjectType == GameObjectType::CHARACTER && b.gameObjectType == GameObjectType::END) ||
		(b.gameObjectType == GameObjectType::CHARACTER && a.gameObjectType == GameObjectType::END);

    bool platformCollision =
        (a.gameObjectType == GameObjectType::PLATFORM && !a.hasCollided) ||
        (b.gameObjectType == GameObjectType::PLATFORM && !b.hasCollided);

	if (isPlayerEndCollision && res) {
        std::cout << "pos 2: " << b.position.x << ", " << b.position.y << "\n";
        std::cout << " pos 1: " << a.position.x << ", " << a.position.y << "\n";
		hasPlayerDied = true;
	}

    if (platformCollision && res) {
        scoreManager.incrementScore(1);
        std::cout << "Score: " << scoreManager.score << "\n";

        if (a.gameObjectType == GameObjectType::PLATFORM && !a.hasCollided) a.hasCollided = true;

        if (b.gameObjectType == GameObjectType::PLATFORM && !b.hasCollided) b.hasCollided = true;
    }

	return { res,aMinX,aMaxX,aMinY,aMaxY,bMinX,bMinY,bMaxX,bMaxY};
}
