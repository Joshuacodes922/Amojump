#include "Scene.hpp"
void Scene::initScene()
{
	defaultScales[GameObjectType::CHARACTER] = { 1.0f, 1.0f, 1.0f };
	defaultScales[GameObjectType::PLATFORM] = { 2.0f, 1.0f, 1.0f };
	defaultScales[GameObjectType::TEST_OBJECT] = { 1.0f, 1.0f, 1.0f };
	defaultScales[GameObjectType::END] = { 100.0f, 23.0f, 1.0f };

	sceneObjects.clear();

	std::vector<SceneObject> tempChunk;
	SceneObject character;
	//character.gameObjectType = GameObjectType::END;
	character.gameObjectType = GameObjectType::CHARACTER;
	character.position = { 0.0f, -30.0f, 0.0f };
	character.rotation = { 0.0f, 0.0f, 0.0f };
	//character.scale = defaultScales[GameObjectType::END];
	character.scale = defaultScales[character.gameObjectType];
	character.gravityAffected = true;
	character.allowCharacterController = true;

	SceneObject end;
	end.gameObjectType = GameObjectType::END;
	end.position = { 0.0f, -20.0f, 1.0f };
	end.rotation = { 0.0f, 0.0f, 0.0f };
	end.scale = defaultScales[end.gameObjectType];
	end.gravityAffected = false;
	end.allowCharacterController = false;

	sceneObjects.push_back(character);
	sceneObjects.push_back(end);

}

int Scene::getCharacterIndex()
{
	for (int i = 0; i < sceneObjects.size(); i++) {
		if (sceneObjects[i].gameObjectType == GameObjectType::CHARACTER) {
			return i;
		}
	}
	return -1;
}


Scene::Scene(GameObjectManager& gameObjectManager, InputManager& inputManager) :gameObjectManager(gameObjectManager),
inputManager(inputManager)
{
	initScene();
}

Scene::~Scene() = default;

void Scene::updateObjectsMovement(float deltaTime)
{

	for (auto& sceneObject : sceneObjects) {

		if (sceneObject.allowCharacterController) {
			updateInputPosition(sceneObject, deltaTime);
		}
		applyGravityToObject(sceneObject, deltaTime);
	}
}
