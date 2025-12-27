#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <GLFW/glfw3.h>
#include <optional>

class Swapchain {
public:

    std::optional<vk::raii::SwapchainKHR> swapchain;
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> swapChainImageViews;

    vk::Format swapChainImageFormat = vk::Format::eUndefined;
    vk::Extent2D swapChainExtent;
    vk::SurfaceFormatKHR swapChainSurfaceFormat;

public:
    Swapchain(
        const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::raii::SurfaceKHR& surface,
        GLFWwindow* window,
		uint32_t &graphicsQueueFamily,
		uint32_t &presentQueueFamily
    );

private:
    void createSwapchain(
        const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::raii::SurfaceKHR& surface,
        GLFWwindow* window
    );

    void createImageViews(const vk::raii::Device& device);

private:
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR>& formats
    );

    vk::PresentModeKHR chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR>& presentModes
    );
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

    /*void cleanupSwapChain();*/

 

public:
    uint32_t graphicsQueueFamily;
    uint32_t presentQueueFamily;
};
