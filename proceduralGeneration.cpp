#include "proceduralGeneration.hpp"
#include "Scene.hpp"
#include "camera.hpp"


ProceduralGeneration::ProceduralGeneration(Scene& scene, Camera& camera):scene(scene),camera(camera)
{
	populateChunksArray();
	offset = -5;
	initChunks();
}

void ProceduralGeneration::destroyAndAddPlatforms(Scene& scene)
{
	
	destroyChunk(scene);
	
	for (auto& i : scene.sceneObjects) {
		if (i.gameObjectType == GameObjectType::END) {
			i.position.y = 10;
		}
	}
}

void ProceduralGeneration::destroyPlatformOnCollision()
{

}

void ProceduralGeneration::addChunk(Scene& scene)
{
	noOfChunksAdded = (noOfChunksAdded+1) % 5;
	if (noOfChunksAdded == 4) {
		scalingFactor *= 1.4;
	}
	//std::cout << "No of chunks added: " << noOfChunksAdded << "\n";
	//std::cout << "Scaling factor: " << scalingFactor << "\n";
	int indexOfChunk = chooseChunk();
	const ChunkPlatform& chunk = chunks[indexOfChunk];
	//std::cout << "Adding chunk at offset: " << offset << std::endl;
	for (auto platform : chunk.platforms) {
		platform.position.y -= offset;
		platform.scale.x /= scalingFactor;
		//std::cout << "Adding platform at y: " << platform.position.y << std::endl;
		scene.addSceneObject(platform);
	}
	offset += 5;
}

void ProceduralGeneration::destroyChunk(Scene& scene)
{
	float worldY = -(camera.viewMatrix[0][1] * camera.viewMatrix[3][0] +
		camera.viewMatrix[1][1] * camera.viewMatrix[3][1] +
		camera.viewMatrix[2][1] * camera.viewMatrix[3][2]);

	float destroyHeight = worldY + 30;
	int count = size(scene.sceneObjects);
	scene.sceneObjects.erase(std::remove_if(scene.sceneObjects.begin(), scene.sceneObjects.end(),
		[destroyHeight](const SceneObject& obj) {

			if (obj.gameObjectType == GameObjectType::END || obj.gameObjectType == GameObjectType::CHARACTER)
				return false;

			return obj.position.y > destroyHeight;
		}
	),
		scene.sceneObjects.end()
	);

	int newCount = size(scene.sceneObjects);
	if (count - newCount == 0) {
		return;
	}
	else {
		int noOfChunks = (noOfDeletedChunks + (count - newCount));

		if (noOfChunks == 3) {
			addChunk(scene);
			noOfDeletedChunks = 0;
		}
		else if(noOfChunks<3) {
			noOfDeletedChunks+=(count-newCount);
		}
		else {
			addChunk(scene);
			noOfDeletedChunks = 0;
		}
		
		std::cout << "No of of objects in scene: " << size(scene.sceneObjects) << "\n";

		
	}
}

void ProceduralGeneration::initChunks()
{
	for (int i = 0; i < 10; i++) {

		addChunk(scene);
	}
}

int ProceduralGeneration::chooseChunk()
{
	//Should be random but for testing purposes always the first chunk
	return 0;
}

void ProceduralGeneration::populateChunksArray()
{
	std::vector<SceneObject> tempChunk;
	SceneObject platform;
	platform.gameObjectType = GameObjectType::PLATFORM;
	platform.position = { -2.0f, 0.0f, 0.0f };
	platform.rotation = { 0.0f, 0.0f, 0.0f };
	platform.scale = scene.defaultScales[platform.gameObjectType];
	tempChunk.push_back(platform);

	SceneObject testObject;
	testObject.gameObjectType = GameObjectType::PLATFORM;
	testObject.position = { 2.0f, 0.0f, 0.0f };
	testObject.rotation = { 0.0f, 0.0f, 0.0f };
	testObject.scale = scene.defaultScales[testObject.gameObjectType];
	tempChunk.push_back(testObject);

	SceneObject platform2;
	platform2.gameObjectType = GameObjectType::PLATFORM;
	platform2.position = { 0.0f, 2.5f, 0.0f };
	platform2.rotation = { 0.0f, 0.0f, 0.0f };
	platform2.scale = scene.defaultScales[platform2.gameObjectType];
	tempChunk.push_back(platform2);

	ChunkPlatform chunk;
	chunk.platforms = tempChunk;
	chunks.push_back(chunk);
}
