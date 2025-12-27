#include "pipeline.hpp"
#include <iostream>

struct pushConstantData {
	alignas(16) glm::mat4 model;
};

Pipeline::Pipeline(Descriptor& descriptor) :descriptor(descriptor)
{
	
}

std::vector<char> Pipeline::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if(!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filename);
	}

	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();

	return buffer;
}

void Pipeline::createGraphicsPipeline(std::optional<Swapchain>& swapChain, std::optional<vk::raii::Device>& device)
{
	
	std::cout << "creating graphics pipeline start" << std::endl;
	auto shaderCode = readFile("shaders/vertex.spv");
	vk::raii::ShaderModule shaderModule = createShaderModule(shaderCode,  *device);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ };
	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = shaderModule;
	vertShaderStageInfo.pName = "main";

	auto shaderCode2 = readFile("shaders/fragment.spv");
	vk::raii::ShaderModule shaderModule2 = createShaderModule(shaderCode2, *device);

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ };
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = shaderModule2;
	fragShaderStageInfo.pName = "main";

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
	std::cout << "creating fixed piprline stage start" << std::endl;
	createFixedPipelineStage(swapChain, device, shaderStages);

}

vk::raii::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code, const vk::raii::Device& device) const
{
	vk::ShaderModuleCreateInfo createInfo{ };
	createInfo.codeSize = code.size() * sizeof(char);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	vk::raii::ShaderModule shaderModule{ device, createInfo };

	return shaderModule;
}

void Pipeline::createFixedPipelineStage(std::optional<Swapchain>& swapChain, std::optional<vk::raii::Device> &device,const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages)
{
	//Choosing the states you dont want to be baked in.
	std::vector dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	
	vk::PipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.setDynamicStates(dynamicStates);

	//Deciding on the format of the data that will be passed into the vertex shader
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vertexInputInfo
		.setVertexBindingDescriptionCount(1)
		.setPVertexBindingDescriptions(&bindingDescription)
		.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()))
		.setPVertexAttributeDescriptions(attributeDescriptions.data());


	//Deciding on what type of geometry is formed
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

	//Defining viewport to show output
	vk::Viewport viewport{ 0.0f, 0.0f, static_cast<float>(swapChain->swapChainExtent.width), static_cast<float>(swapChain->swapChainExtent.height), 0.0f, 1.0f };

	//Defining scissor to decide how much of the image is shown
	vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, swapChain->swapChainExtent };

	//Deciding on the vieportCounts
	vk::PipelineViewportStateCreateInfo viewportState{ };
	viewportState.setScissorCount(1);
	viewportState.setViewportCount(1);
	viewportState.setViewports({ viewport });
	viewportState.setScissors({ scissor });

	//Rasterization stage
	vk::PipelineRasterizationStateCreateInfo rasterizer({}, vk::False, vk::False, vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, vk::False, 0.0f, 0.0f, 1.0f, 1.0f);

	rasterizer.lineWidth = 1.0;
	//Multisampling - Helps with antialiasing
	vk::PipelineMultisampleStateCreateInfo multisampling{};
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multisampling.setSampleShadingEnable(vk::False);

	//Color Blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.setBlendEnable(VK_TRUE)
		.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
		.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
		.setColorBlendOp(vk::BlendOp::eAdd)
		.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
		.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
		.setAlphaBlendOp(vk::BlendOp::eAdd)
		.setColorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA
		);
	vk::PipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.setLogicOp(vk::LogicOp::eCopy);
	colorBlending.setAttachments(colorBlendAttachment); // sets attachmentCount and pAttachments
	colorBlending.setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f }); // optional

	vk::PushConstantRange pushConstantRange;
	pushConstantRange
		.setStageFlags(vk::ShaderStageFlagBits::eVertex)
		.setOffset(0)
		.setSize(sizeof(glm::mat4)); // model matrix

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo
		.setSetLayoutCount(1)
		.setPSetLayouts(&*(descriptor.descriptorSetLayout.value()))
		.setPushConstantRangeCount(1)
		.setPPushConstantRanges(&pushConstantRange);

	pipelineLayout.emplace(*device, pipelineLayoutInfo);

	//Dynamic rendering
	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{ };
	pipelineRenderingCreateInfo.setColorAttachmentCount(1);
	pipelineRenderingCreateInfo.setColorAttachmentFormats({ swapChain->swapChainImageFormat });
	//Configuring the pipeline creation Info
	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo
		.setPNext(&pipelineRenderingCreateInfo)
		.setStages(shaderStages) 
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssembly)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizer)
		.setPMultisampleState(&multisampling)
		.setPColorBlendState(&colorBlending)
		.setPDynamicState(&dynamicState)
		.setLayout(*pipelineLayout)
		.setRenderPass(nullptr)
		.setSubpass(0); 
	
	
	//IMPORTANT : CHECK IF THE ENTRY POINT(pname) OF THE SHADER IS THE SAME AS WHAT SLANG HAS DECIDED ON YOUR SYSTEM. 
	pipeline.emplace(*device, nullptr, pipelineInfo);
	
	
	
}


