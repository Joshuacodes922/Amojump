#pragma once
#include <vector>
#include <memory>
class Scene;
struct SceneObject;
class Camera;

struct ChunkPlatform {
	std::vector<SceneObject> platforms;
};

class ProceduralGeneration {
		int offset = 10;
		int start;
		int noOfDeletedChunks = 0;
		int noOfChunksAdded = 0;
		float scalingFactor = 1;
		
	public:
		Scene& scene;
		Camera& camera;
		std::vector<ChunkPlatform> chunks;
		ProceduralGeneration(Scene& scene,Camera& camera);

		void destroyAndAddPlatforms(Scene& scene);
		void destroyPlatformOnCollision();
		void addChunk(Scene& scene);
		void destroyChunk(Scene& scene);
		void initChunks();
		int chooseChunk();
		void populateChunksArray();
};