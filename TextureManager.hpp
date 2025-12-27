#pragma once

#include "texture.hpp"
#include "buffer.hpp"
class TextureManager {
public:
	uint32_t load(const std::string& path);
    Texture& get(uint32_t id);

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = default;
	TextureManager& operator=(TextureManager&&) = default;

	TextureManager(Buffer& buffer, vk::raii::Queue& graphicsQueue, vk::raii::Device& device);
	std::vector<Texture> textures;

	std::array<std::string, 5> texturesUrl = {
		"textures/platform.png",
		"textures/character.png",
		"textures/texture.jpg",
		"textures/texture2.jpg",
		"textures/end.png"
	};
private:
    
	vk::raii::Device& device;
	vk::raii::Queue& graphicsQueue;
	Buffer& buffer;
};