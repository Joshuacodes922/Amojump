#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <glm/glm.hpp>
#include <optional>
#include "buffer.hpp"
#include <iostream>
#include "TextureManager.hpp"
#include "sampler.hpp"
class Descriptor {
    void createDescriptorSetLayout();

public:
    std::optional<vk::raii::DescriptorSetLayout> descriptorSetLayout;
	std::optional<vk::raii::DescriptorPool> descriptorPool;
    std::vector<std::vector<vk::raii::DescriptorSet>> descriptorSets;

	Buffer& buffer;
    vk::raii::Device& device;
	TextureManager& textureManager;
    Descriptor(vk::raii::Device& device,Buffer& buffer, TextureManager& textureManager);
    
	void createDescriptorPool(int MAX_FRAMES_IN_FLIGHT);
	void createDescriptorSets(int MAX_FRAMES_IN_FLIGHT, TextureManager& textureManager, Sampler& sampler);
};

