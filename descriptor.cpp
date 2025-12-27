#include "descriptor.hpp"


Descriptor::Descriptor(vk::raii::Device& device, Buffer& buffer, TextureManager& textureManager) :device(device), buffer(buffer), textureManager(textureManager)
{
	createDescriptorSetLayout(); 
}

void Descriptor::createDescriptorPool(int MAX_FRAMES_IN_FLIGHT)
{
	
	std::array<vk::DescriptorPoolSize, 2> poolSize{
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) },
		vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * textureManager.textures.size()) }
	};
	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setMaxSets(MAX_FRAMES_IN_FLIGHT * static_cast<uint32_t>(textureManager.textures.size()))
		.setPoolSizeCount(static_cast<uint32_t>(poolSize.size()))
		.setPPoolSizes(poolSize.data());

	descriptorPool.emplace(device, poolInfo);
}

void Descriptor::createDescriptorSets(int MAX_FRAMES_IN_FLIGHT, TextureManager& textureManager, Sampler& sampler)
{
	if (!descriptorSetLayout.has_value()) {
		throw std::runtime_error("descriptorSetLayout not created before createDescriptorSets()");
	}
	if (!descriptorPool.has_value()) {
		throw std::runtime_error("descriptorPool not created before createDescriptorSets()");
	}
	
	for (auto& texture : textureManager.textures) {
		std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
		vk::DescriptorSetAllocateInfo allocInfo;
		allocInfo.setDescriptorPool(*descriptorPool)
			.setDescriptorSetCount(static_cast<uint32_t>(layouts.size()))
			.setPSetLayouts(layouts.data());


		std::vector<vk::raii::DescriptorSet> textureDescriptorSets = device.allocateDescriptorSets(allocInfo);
		

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo.setBuffer(*buffer.uniformBuffers[i])
				.setOffset(0)
				.setRange(sizeof(UniformBufferObject));

			vk::DescriptorImageInfo imageInfo;
			imageInfo.setSampler(*sampler.linearRepeat)
				.setImageView(*texture.textureImageView)
				.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

			std::array<vk::WriteDescriptorSet, 2> descriptorWrites;

			descriptorWrites[0]
				.setDstSet(textureDescriptorSets[i])
				.setDstBinding(0)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setPBufferInfo(&bufferInfo);

			descriptorWrites[1]
				.setDstSet(textureDescriptorSets[i])
				.setDstBinding(1)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setPImageInfo(&imageInfo);


			device.updateDescriptorSets(descriptorWrites, {});
		}
		descriptorSets.push_back(std::move(textureDescriptorSets));
	}
	

}

void Descriptor::createDescriptorSetLayout()
{
	std::array bindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)
	};

	vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings.size(), bindings.data());

	descriptorSetLayout.emplace(device, layoutInfo);
}


