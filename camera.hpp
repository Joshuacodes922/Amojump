#include <glm/glm.hpp>


class Scene;

class Camera {
public:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	Scene& scene;
	void updateViewMatrix();

	Camera(Scene& scene):scene(scene) {
		viewMatrix = glm::mat4(1.0f);
		updateViewMatrix();
	}
	
};