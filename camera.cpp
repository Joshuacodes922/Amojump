#include "camera.hpp"
#include "Scene.hpp"

void Camera::updateViewMatrix()
{
	int playerIndex = scene.getCharacterIndex();

	if (playerIndex != -1) {
		const SceneObject& player = scene.sceneObjects[playerIndex];
		glm::vec3 cameraPosition = player.position + glm::vec3(0.0f, 2.0f, 5.0f);
		glm::vec3 targetPosition = player.position + glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 upDirection = glm::vec3(0.0f, -1.0f, 0.0f);
		viewMatrix = glm::lookAt(cameraPosition, targetPosition, upDirection);
	}
}
