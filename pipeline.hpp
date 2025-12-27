#include <fstream>
#include <vector>
#include <array>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <GLFW/glfw3.h>
#include <optional>
#include "swapchain.hpp"
#include <glm/glm.hpp>
#include "MeshManager.hpp"
#include "descriptor.hpp"


class Pipeline {
public:
	Pipeline(Descriptor& descriptor);
	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;

	static std::vector<char> readFile(const std::string& filename);

	void createGraphicsPipeline(std::optional<Swapchain>& swapChain, std::optional<vk::raii::Device>& device);
	

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, const vk::raii::Device& device) const;

	void createFixedPipelineStage(std::optional<Swapchain>& swapChain, std::optional<vk::raii::Device> &device,const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages);

	std::optional<vk::raii::PipelineLayout> pipelineLayout;

	std::optional<vk::raii::Pipeline> pipeline;

	Descriptor& descriptor;
};