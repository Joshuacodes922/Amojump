#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include "texture.hpp"
#include <unordered_map>

enum class GameObjectType {
    CHARACTER,
    PLATFORM,
    TEST_OBJECT,
    END
};



struct GameObject {
    std::string name;
    uint32_t textureId;
	uint32_t meshId;

};

class GameObjectManager {
    void initGameObjects();

public:
    GameObjectManager();
    std::unordered_map<GameObjectType, int> objectMap;
    std::array<GameObject, 4> gameObjects;
	
};