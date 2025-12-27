#pragma once
#include <vector>
#include <string>
#include "GameObject.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include "inputManager.hpp"
#include <iostream>

#include <optional>
#include <memory>


struct SceneObject {
	std::string name;
	GameObjectType gameObjectType;
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

	bool gravityAffected = false;
	bool allowCharacterController = false;
	bool isGrounded = true;
	bool hasCollided = false;

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		return model;
	}
};

class Scene {
	void initScene();
	GameObjectManager& gameObjectManager;
	InputManager& inputManager;
	

public:
	//ProceduralGeneration& proceduralGeneration;


	int getCharacterIndex();
	std::vector<SceneObject> sceneObjects;

	std::unordered_map<GameObjectType, glm::vec3> defaultScales;

	void addSceneObject(SceneObject sceneObject) {
		sceneObjects.push_back(sceneObject);
	}



	SceneObject& getSceneObject(uint16_t index) {
		if (index < sceneObjects.size()) {
			return sceneObjects[index];
		}
	}

	

	Scene(GameObjectManager& gameObjectManager,InputManager& inputManager);
	~Scene();

	void updateObjectsMovement(float deltaTime);
	
	void applyGravityToObject(SceneObject& sceneObject, float deltaTime)
	{
		if (sceneObject.gravityAffected) {
			sceneObject.velocity.y += 18.81f * deltaTime;
			sceneObject.position.y += sceneObject.velocity.y * deltaTime;
		}
	}

	void updateInputPosition(SceneObject& sceneObject, float deltaTime) {


		if (inputManager.checkAction(Actions::MOVE_LEFT)) {
			sceneObject.position.x += 5.0f * deltaTime;
		}

		if (inputManager.checkAction(Actions::MOVE_RIGHT)) {
			sceneObject.position.x -= 5.0f * deltaTime;
		}

		if (inputManager.checkAction(Actions::JUMP) && sceneObject.isGrounded) {
			
			sceneObject.velocity.y = -12.0f;
			sceneObject.isGrounded = false;
			
		}
	}
};