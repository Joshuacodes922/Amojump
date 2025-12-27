#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <optional>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "swapchain.hpp"

#include <chrono>
#include <cstring> 
struct Vertex;
class Camera;
struct UniformBufferObject {
	glm::mat4 view;
	glm::mat4 proj;
};

class Buffer {
public:
	struct AllocateBuffer {
		vk::raii::Buffer buffer;
		vk::raii::DeviceMemory bufferMemory;
	};

	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;


	vk::raii::PhysicalDevice& physicalDevice;
	vk::raii::Device& device;
	vk::raii::CommandPool& commandPool;
	vk::raii::Queue& graphicsQueue;
	Swapchain& swapchain;

	int MAX_FRAMES_IN_FLIGHT;

public:
	Buffer(vk::raii::PhysicalDevice& physicalDevice,
		vk::raii::Device& device,
		vk::raii::CommandPool& commandPool,
		vk::raii::Queue& graphicsQueue,
		int MAX_FRAMES_IN_FLIGHT,
		Swapchain& swapchain);

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;
	
	Buffer(Buffer&&) = default;
	Buffer& operator=(Buffer&&) = default;

	Buffer::AllocateBuffer createVertexBufer(const std::vector<Vertex>& vertices);
	Buffer::AllocateBuffer createIndexBuffer(const std::vector<uint16_t>& indices);
	void createUniformBuffers();

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, 
		std::optional< vk::raii::Buffer>& buffer, std::optional<vk::raii::DeviceMemory>& bufferMemory);

	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);
	void updateUniformBuffer(uint32_t frameIndex, Camera& camera);

};