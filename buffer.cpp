#include "buffer.hpp"
#include "meshManager.hpp"
#include <iostream>
#include "camera.hpp"
Buffer::Buffer(vk::raii::PhysicalDevice& physicalDevice,
	vk::raii::Device& device,
	vk::raii::CommandPool& commandPool,
	vk::raii::Queue& graphicsQueue,
	int MAX_FRAMES_IN_FLIGHT,
	Swapchain& swapchain)
    : physicalDevice(physicalDevice), device(device),commandPool(commandPool),graphicsQueue(graphicsQueue),MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT),swapchain(swapchain)
{
	createUniformBuffers();
}

Buffer::AllocateBuffer Buffer::createVertexBufer(const std::vector<Vertex>& vertices)
{
	std::optional<vk::raii::Buffer> vertexBuffer;
	std::optional<vk::raii::DeviceMemory> vertexBufferMemory;
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	//Staging buffer
	std::optional<vk::raii::Buffer> stagingBuffer;
	std::optional<vk::raii::DeviceMemory> stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = stagingBufferMemory->mapMemory(0, bufferSize);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	stagingBufferMemory->unmapMemory();

	//Vertex Buffer init
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
	
	copyBuffer(*stagingBuffer, *vertexBuffer, bufferSize);

	return Buffer::AllocateBuffer{ std::move(*vertexBuffer), std::move(*vertexBufferMemory) };
}

Buffer::AllocateBuffer Buffer::createIndexBuffer(const std::vector<uint16_t>& indices)
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	std::optional<vk::raii::Buffer> indexBuffer;
	std::optional<vk::raii::DeviceMemory> indexBufferMemory;

	std::optional<vk::raii::Buffer> stagingBuffer({});
	std::optional<vk::raii::DeviceMemory> stagingBufferMemory({});
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = stagingBufferMemory->mapMemory(0, bufferSize);
	memcpy(data, indices.data(), (size_t)bufferSize);
	stagingBufferMemory->unmapMemory();

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(*stagingBuffer, *indexBuffer, bufferSize);

	return Buffer::AllocateBuffer{ std::move(*indexBuffer), std::move(*indexBufferMemory) };
}

void Buffer::createUniformBuffers()
{
	uniformBuffers.clear();
	uniformBuffersMemory.clear();
	uniformBuffersMapped.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		std::optional<vk::raii::Buffer> buffer({});
		std::optional<vk::raii::DeviceMemory> bufferMem({});
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, buffer, bufferMem);
		uniformBuffers.emplace_back(std::move(*buffer));
		uniformBuffersMemory.emplace_back(std::move(*bufferMem));
		uniformBuffersMapped.emplace_back(uniformBuffersMemory[i].mapMemory(0, bufferSize));
	}

}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Buffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, std::optional< vk::raii::Buffer>& buffer, std::optional<vk::raii::DeviceMemory>& bufferMemory)
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo
		.setSize(size)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive);
	buffer.emplace(device, bufferInfo);

	vk::MemoryRequirements memRequirements = buffer->getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo
		.setAllocationSize(memRequirements.size)
		.setMemoryTypeIndex(
			findMemoryType(
				memRequirements.memoryTypeBits,
				properties
			)
		);


	bufferMemory.emplace(device, memoryAllocateInfo);
	buffer->bindMemory(*bufferMemory, 0);
}

void Buffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount(1);

	vk::raii::CommandBuffer commandCopyBuffer =
		std::move(device.allocateCommandBuffers(allocInfo).front());

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	commandCopyBuffer.begin(beginInfo);
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1)
		.setPCommandBuffers(&*commandCopyBuffer);

	graphicsQueue.submit(submitInfo, nullptr);
	graphicsQueue.waitIdle();

}

void Buffer::updateUniformBuffer(uint32_t frameIndex,Camera& camera)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	//ubo.view = lookAt(glm::vec3(0.0f, 5.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	ubo.view = camera.viewMatrix;
	float width = static_cast<float>(std::max(1u, swapchain.swapChainExtent.width));
	float height = static_cast<float>(std::max(1u, swapchain.swapChainExtent.height));
	float aspect = width / height;
	float size = 10.0f;

	ubo.proj = glm::ortho(
		-size * aspect,
		size * aspect,
		-size,
		size,
		0.1f,
		20.0f
	);

	// Vulkan clip-space fix
	ubo.proj[1][1] *= -1;

	memcpy(uniformBuffersMapped[frameIndex], &ubo, sizeof(ubo));

}
