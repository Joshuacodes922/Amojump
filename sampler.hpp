#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <optional>
#pragma once
class Sampler {
	void createLinearRepeatTextureSampler();
	vk::raii::Device& device;
	vk::raii::PhysicalDevice& physicalDevice;
public:
	std::optional<vk::raii::Sampler> linearRepeat;

    Sampler(vk::raii::Device& device,
        vk::raii::PhysicalDevice& physicalDevice);

	Sampler(const Sampler&) = delete;
	Sampler& operator=(const Sampler&) = delete;

	Sampler(Sampler&&) = default;
	Sampler& operator=(Sampler&&) = default;
};