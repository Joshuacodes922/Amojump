#include "gameobject.hpp"

void GameObjectManager::initGameObjects()
{
	gameObjects[0].name = "Character";
	gameObjects[0].textureId = 1;
	gameObjects[0].meshId = 0;

	objectMap[GameObjectType::CHARACTER] = 0;

    gameObjects[1].name = "Platform";
    gameObjects[1].textureId = 0;
	gameObjects[1].meshId = 0;

	objectMap[GameObjectType::PLATFORM] = 1;

    gameObjects[2].name = "test object";
    gameObjects[2].textureId = 2;
	gameObjects[2].meshId = 0;

	objectMap[GameObjectType::TEST_OBJECT] = 2;

	gameObjects[3].name = "End";
	gameObjects[3].textureId = 4;
	gameObjects[3].meshId = 0;

	objectMap[GameObjectType::END] = 3;
}

GameObjectManager::GameObjectManager()
{
	initGameObjects();
}
