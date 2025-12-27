#include "meshManager.hpp"

MeshManager::Mesh MeshManager::loadMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
	Mesh mesh1;
	Buffer::AllocateBuffer vertexBufferAlloc = bufferManager.createVertexBufer(vertices);
	Buffer::AllocateBuffer indexBufferAlloc = bufferManager.createIndexBuffer(indices);
	mesh1.vertexBuffer = std::move(vertexBufferAlloc.buffer);
	mesh1.vertexBufferMemory = std::move(vertexBufferAlloc.bufferMemory);
	mesh1.indexBuffer = std::move(indexBufferAlloc.buffer);
	mesh1.indexBufferMemory = std::move(indexBufferAlloc.bufferMemory);
	mesh1.indexCount = static_cast<uint16_t>(indices.size());

	std::cout << "Loaded mesh with " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;
	return mesh1;
}

void MeshManager::initMeshes()
{
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} 
	};
	const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
	};

	meshes.push_back(loadMesh(vertices, indices));
}
