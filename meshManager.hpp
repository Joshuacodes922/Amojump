
#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include "buffer.hpp"
#include <iostream>
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription getBindingDescription() {
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
			vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
		};
	}
};


class MeshManager {
	Buffer& bufferManager;
	public:
	struct Mesh {

		std::optional<vk::raii::Buffer> vertexBuffer;
		std::optional<vk::raii::Buffer> indexBuffer;
		std::optional<vk::raii::DeviceMemory> vertexBufferMemory;
		std::optional<vk::raii::DeviceMemory> indexBufferMemory;
		uint16_t indexCount;
	};
	std::vector<Mesh> meshes;
	MeshManager(Buffer &bufferManager):bufferManager(bufferManager) {
		initMeshes();
	}

	Mesh& getMesh(uint32_t id) {
		return meshes[id];
	}

	

	Mesh loadMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);

	MeshManager(const MeshManager&) = delete;
	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = default;
	MeshManager& operator=(MeshManager&&) = default;

private:
	void initMeshes();
};