#include "texture.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

void Texture::createTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(imageUrl.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image! ");
    }

    //Staging buffer
    std::optional<vk::raii::Buffer> stagingBuffer;
    std::optional<vk::raii::DeviceMemory> stagingBufferMemory;

    buffer.createBuffer(
        imageSize, 
        vk::BufferUsageFlagBits::eTransferSrc, 
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data = stagingBufferMemory->mapMemory(0, imageSize);
    memcpy(data, pixels, (size_t)imageSize);
    stagingBufferMemory->unmapMemory();

    stbi_image_free(pixels);

    createImage(
        texWidth, 
        texHeight, 
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal, 
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal, 
        textureImage, textureImageMemory);
    
    transitionImageLayout(*textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(*stagingBuffer, *textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(*textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Texture::createTextureImageView()
{
    textureImageView = createImageView(*textureImage, vk::Format::eR8G8B8A8Srgb);
}


vk::raii::CommandBuffer Texture::beginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo
        .setCommandPool(buffer.commandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);

    vk::raii::CommandBuffer commandBuffer = std::move(buffer.device.allocateCommandBuffers(allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);

    return commandBuffer;

}

void Texture::endSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo
        .setCommandBufferCount(1)
        .setPCommandBuffers(&*commandBuffer);

    graphicsQueue.submit(submitInfo, nullptr);
    graphicsQueue.waitIdle();
}

void Texture::createImage(
    uint32_t width, 
    uint32_t height, 
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage, 
    vk::MemoryPropertyFlags properties,
    std::optional<vk::raii::Image>& image,
    std::optional<vk::raii::DeviceMemory>& imageMemory)
{
    vk::ImageCreateInfo imageInfo{};
    imageInfo
        .setImageType(vk::ImageType::e2D)
        .setFormat(format)
        .setExtent({width,height, 1 })
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(tiling)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);

    image.emplace(buffer.device, imageInfo);

    vk::MemoryRequirements memRequirements = textureImage->getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo
        .setAllocationSize(memRequirements.size)
        .setMemoryTypeIndex(
            buffer.findMemoryType(memRequirements.memoryTypeBits, properties)
        );
	imageMemory.emplace(buffer.device, allocInfo);
    textureImage->bindMemory(*textureImageMemory, 0);
}

void Texture::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
    vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands();
    commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
    endSingleTimeCommands(commandCopyBuffer);
}

void Texture::copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height)
{
    vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferImageCopy region{};
    region.setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource(
            vk::ImageSubresourceLayers()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setMipLevel(0)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
        )
        .setImageOffset({ 0, 0, 0 })
        .setImageExtent({ width, height, 1 });
    
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });
    endSingleTimeCommands(commandBuffer);
}

void Texture::transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{};
    barrier.setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setImage(image)
        .setSubresourceRange(
            vk::ImageSubresourceRange()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
        );
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);

    endSingleTimeCommands(commandBuffer);
}

vk::raii::ImageView Texture::createImageView(vk::raii::Image& image, vk::Format format) {

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format)
        .setSubresourceRange(
            vk::ImageSubresourceRange{}
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
        );

    return vk::raii::ImageView(device, viewInfo);
}


void Texture::createImageViews()
{
    
}

Texture::Texture(Buffer& buffer, vk::raii::Queue& graphicsQueue, vk::raii::Device& device, std::string imageUrl) :buffer(buffer), graphicsQueue(graphicsQueue), imageUrl(imageUrl),
device(device){
    createTextureImage();
    createTextureImageView();
}


