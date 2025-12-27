#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include"buffer.hpp"
#include <optional>


class Texture {
	void createTextureImage();
	void createTextureImageView();
	

	vk::raii::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer);
	void createImage(uint32_t width, uint32_t height, 
		vk::Format format, vk::ImageTiling tiling, 
		vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
		std::optional<vk::raii::Image>& image, std::optional<vk::raii::DeviceMemory>& imageMemory);

	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);
	void copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);
	void transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	vk::raii::ImageView createImageView(vk::raii::Image& image, vk::Format format);
	void createImageViews();

	Buffer& buffer;
	vk::raii::Device& device;
	vk::raii::Queue& graphicsQueue;
	std::string imageUrl;
	
public:
	std::optional<vk::raii::Image> textureImage;
	std::optional<vk::raii::DeviceMemory> textureImageMemory;
	std::optional<vk::raii::ImageView> textureImageView;

	Texture(Buffer &buffer, vk::raii::Queue& graphicsQueue,vk::raii::Device& device,std::string imageUrl);

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;



};