#include "TextureManager.hpp"

uint32_t TextureManager::load(const std::string& path)
{

	for (auto& textureUrl : texturesUrl) {
		Texture texture1(buffer, graphicsQueue, device,textureUrl);
		textures.push_back(std::move(texture1));
	}
	return static_cast<uint32_t>(textures.size() - 1);
}

Texture& TextureManager::get(uint32_t id)
{
	return textures.at(id);
}

TextureManager::TextureManager(Buffer& buffer, vk::raii::Queue& graphicsQueue, vk::raii::Device& device) : buffer(buffer), graphicsQueue(graphicsQueue), device(device)
{
	load("textures/character.png");
}
