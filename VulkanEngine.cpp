// Vulkan Engine Cmake Build.cpp : Defines the entry point for the application.
// ""C:/Users/JoshuaS/source/repos/Vulkan Engine Cmake Build/dependencies/slang-2025.23.1-windows-x86_64/bin/slangc.exe" "C:/Users/JoshuaS/source/repos/Vulkan Engine Cmake Build/shaders/vertex.slang" -target spirv -o "C:/Users/JoshuaS/source/repos/Vulkan Engine Cmake Build/shaders/vertex.spv"

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <memory>
#include <optional>
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "buffer.hpp"
#include "descriptor.hpp"
#include "TextureManager.hpp"
#include "sampler.hpp"
#include "gameobject.hpp"
#include "meshManager.hpp"
#include "Scene.hpp"
#include "collisions.hpp"
#include "camera.hpp"
#include <chrono>
#include "proceduralGeneration.hpp"
#include "scoreManager.hpp"
const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif



constexpr int MAX_FRAMES_IN_FLIGHT = 2;
struct QueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
	bool isComplete() const {
		return graphicsFamily != UINT32_MAX &&
			presentFamily != UINT32_MAX;
	}
};

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
class HelloTriangle {

	public:
	
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	private:

		GLFWwindow* window;
		std::optional<vk::raii::Context> context;
		std::optional<vk::raii::Instance> instance;
		std::optional<vk::raii::PhysicalDevice> physicalDevice;
		std::optional<vk::raii::Device> device;
		std::optional<vk::raii::Queue> graphicsQueue;
		std::optional<vk::raii::Queue> presentQueue;
		std::optional<vk::raii::SurfaceKHR> surface;


		std::unique_ptr<Descriptor> descriptor;

		std::optional< vk::raii::CommandPool> commandPool;
		std::optional< vk::raii::CommandBuffers> commandBuffers;
		/*std::vector<vk::raii::CommandBuffer> commandBuffers;*/
		ScoreManager scoreManager;
		std::optional<Swapchain> swapChain;
		std::optional<CollisionHandler> collisionHandler;
		std::optional<Buffer> buffers;

		std::unique_ptr<TextureManager> textureManager;
		std::optional<ProceduralGeneration> proceduralGeneration;
		std::unique_ptr<MeshManager> meshManager;
		std::unique_ptr<Scene> scene;
		std::optional<Camera> camera;
		std::optional<GameObjectManager> gameObjectManager;
		std::optional<InputManager> inputManager;
		std::optional<Sampler> textureSampler;
		uint32_t presentFamily,graphicsFamily;

		std::optional<Pipeline> pipeline;

		//Semaphores
		//std::optional< vk::raii::Semaphore> presentCompleteSemaphore, renderFinishedSemaphore;
		std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
		std::vector<vk::raii::Semaphore> renderFinishedSemaphores;


		//Fence
		std::vector<vk::raii::Fence> inFlightFences;

		uint32_t currentFrame = 0;
		bool framebufferResized = false;
		using Clock = std::chrono::high_resolution_clock;
		Clock::time_point lastTime = std::chrono::high_resolution_clock::now();
		

		void initVulkan() {
			createInstance();
			createInputHandler();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
			createSwapChain();
			createCommandPool();
			initGameObjects();
			initGame();
			createCollisionHandler();
			initScoreManager();
			createBuffers();
			initMeshManager();
			createTextureManager();
			createTextureImageSample();
			createDescriptorSetLayout();
			createGraphicsPipeline();
			createDescriptorPool();
			createDescriptorSets();
			createCommandBuffer();
			createSyncObjects();
		}
		void mainLoop() {
			while (!glfwWindowShouldClose(window)) {
				glfwPollEvents();
				drawFrame();
			}
			device->waitIdle();
		}
		void cleanup() {
			glfwDestroyWindow(window);
			glfwTerminate();
		}
		void initWindow() {
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
			glfwSetWindowUserPointer(window, this);
			glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		}

		void initGame(){
			initScene();
			setupCamera();
			setupProceduralGeneration();
		}
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
			auto app = reinterpret_cast<HelloTriangle*>(glfwGetWindowUserPointer(window));
			app->framebufferResized = true;
		}
		inline float getDeltaTime(auto lastTime) {

			auto currentTime = Clock::now();

			float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

			return deltaTime;
		}
		void drawFrame() {

			if (collisionHandler->hasPlayerDied)
			{
				std::cout << "Player has died, resetting game state." << std::endl;

				if (scoreManager.score > scoreManager.highScore) scoreManager.setHighScore(scoreManager.score);
				scoreManager.score = 0;

				std::cout << "Your score: " << scoreManager.score << "\n";
				std::cout << "Your high score: " << scoreManager.highScore << "\n";
				initGame();
				collisionHandler->hasPlayerDied = false;
			}
		
			// Defensive validation: throw clear errors for missing initialization
			if (!device) throw std::runtime_error("device not initialized");
			if (!graphicsQueue) throw std::runtime_error("drawFrame: graphicsQueue not initialized");
			if (!presentQueue) throw std::runtime_error("drawFrame: presentQueue not initialized");
			if (!commandBuffers) throw std::runtime_error("drawFrame: commandBuffers not initialized");
			if (!swapChain) throw std::runtime_error("drawFrame: swapChain not initialized");
			if (!buffers) throw std::runtime_error("drawFrame: buffers not initialized");
			if (!textureManager) throw std::runtime_error("drawFrame: textureManager not initialized");
			if (!pipeline) throw std::runtime_error("drawFrame: pipeline not initialized");
			if (!descriptor) throw std::runtime_error("drawFrame: descriptor not initialized");	
			//assert(!gameObjectManager->gameObjects.empty());
			if (presentCompleteSemaphores.size() <= currentFrame ||
				renderFinishedSemaphores.size() <= currentFrame ||
				inFlightFences.size() <= currentFrame)
			{
				throw std::runtime_error("drawFrame: sync objects missing for current frame");
			}
			assert(!gameObjectManager->gameObjects.empty() && "No game objects to draw!");
			assert(camera && "Camera not initialized!");
			device->waitForFences({ inFlightFences[currentFrame] }, VK_TRUE, UINT64_MAX);
			auto [result, imageIndex] = swapChain->swapchain->acquireNextImage(UINT64_MAX, presentCompleteSemaphores[currentFrame], nullptr);
			if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
				framebufferResized = false;
				recreateSwapChain();
				return;
			}
		
			if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
				throw std::runtime_error("failed to acquire swap chain image!");
			}

			
			recordCommandBuffer(imageIndex);
			device->resetFences({ *inFlightFences[currentFrame]});
			camera->updateViewMatrix();
			buffers->updateUniformBuffer(currentFrame,*camera);
			
	
			vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
			vk::SubmitInfo submitInfo(
				1, &(*presentCompleteSemaphores[currentFrame]),
				&waitDestinationStageMask,
				1, &(*commandBuffers->at(currentFrame)),
				1, &(*renderFinishedSemaphores[currentFrame])
			);
			float deltaTime = getDeltaTime(lastTime);
			lastTime = Clock::now();  
			
			
			scene->updateObjectsMovement(deltaTime);
			collisionHandler->resolveCollisions(scene->sceneObjects);
			graphicsQueue->submit(submitInfo, *inFlightFences[currentFrame]);
			proceduralGeneration->destroyAndAddPlatforms(*scene);
		
			//const vk::PresentInfoKHR presentInfoKHR(*renderFinishedSemaphores[currentFrame], **swapChain->swapchain, imageIndex);
			vk::PresentInfoKHR presentInfoKHR(
				1, &(*renderFinishedSemaphores[currentFrame]),
				1, &(**swapChain->swapchain),
				&imageIndex
			);

			result = presentQueue->presentKHR(presentInfoKHR);
		
			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		}

		void createSyncObjects() {
			presentCompleteSemaphores.clear();
			renderFinishedSemaphores.clear();
			inFlightFences.clear();
			

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				presentCompleteSemaphores.emplace_back(*device, vk::SemaphoreCreateInfo());
				renderFinishedSemaphores.emplace_back(*device, vk::SemaphoreCreateInfo());
				inFlightFences.emplace_back(*device, vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
			}

			std::cout << "Creating  sync objects finished" << std::endl;
		}

		void createInstance() {
			uint32_t apiVersion = 0;

			// Vulkan >= 1.1
			if (vk::enumerateInstanceVersion(&apiVersion) == vk::Result::eSuccess) {
				uint32_t major = VK_VERSION_MAJOR(apiVersion);
				uint32_t minor = VK_VERSION_MINOR(apiVersion);
				uint32_t patch = VK_VERSION_PATCH(apiVersion);

				std::cout << "Vulkan Instance Version: "
					<< major << "." << minor << "." << patch << std::endl;
			}
			else {
				std::cout << "Failed to query Vulkan instance version!" << std::endl;
			}
			std::cout << "Creating Instance";
			context.emplace();
			 constexpr vk::ApplicationInfo appInfo{"Hello Triangle",
				VK_MAKE_VERSION(1, 0, 0),
				"No Engine",
				VK_MAKE_VERSION(1, 0, 0),
				vk::ApiVersion14
			};

			 //Setting up validation Layers
			 std::vector<const char*> requiredLayers;

			 if(enableValidationLayers) {
				 requiredLayers.assign(validationLayers.begin(), validationLayers.end());
			 }
			 auto layerProperties = context->enumerateInstanceLayerProperties();
			 if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
				 return std::ranges::none_of(layerProperties,
					 [requiredLayer](auto const& layerProperty)
					 { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
				 }))
			 {
				 throw std::runtime_error("One or more required layers are not supported!");
			 }

			 uint32_t glfwExtensionCount = 0;
			 auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			 // Check if the required GLFW extensions are supported by the Vulkan implementation.
			 auto extensionProperties = context->enumerateInstanceExtensionProperties();
			 for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			 {
				 if (std::ranges::none_of(extensionProperties,
					 [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
					 { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
				 {
					 throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
				 }
			 }
			 vk::InstanceCreateInfo createInfo(
				 {},           
				 &appInfo,     
				 0,            
				 requiredLayers.data(),
				 glfwExtensionCount,            
				 glfwExtensions    
			 );

			 std::cout << "Creating Vulkan Instance..." << std::endl;
			 try {
				 
				 instance.emplace(*context, createInfo);
			 }
			 catch(const vk::SystemError &e) {
				 throw std::runtime_error("Failed to create Vulkan instance: " + std::string(e.what()));

			 }
			 catch(const std::exception &e) {
				 throw std::runtime_error("Failed to create Vulkan instance: " + std::string(e.what()));
			 }
			 
			 std::cout << "Creating Instance finished" <<std::endl;
		}

		void pickPhysicalDevice() {
			
			auto devices = instance->enumeratePhysicalDevices();

			std::multimap<int, vk::raii::PhysicalDevice> candidates;

			if (devices.empty()) {
				throw std::runtime_error("failed to find GPUs with Vulkan support!");
			}
			// Just pick the first device for now
			for (const auto& device : devices) {
				auto deviceProperties = device.getProperties();
				auto deviceFeatures = device.getFeatures();
				uint32_t score = 0;
				
				if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
					score += 1000;
				}

				score += deviceProperties.limits.maxImageDimension2D;
				if (!deviceFeatures.geometryShader) {
					continue;
				}
				candidates.insert(std::make_pair(score, device));
			}

			if (candidates.rbegin()->first > 0) {
				physicalDevice = candidates.rbegin()->second;
			}
			else {
				throw std::runtime_error("failed to find a suitable GPU!");
			}

			std::cout << "Selected GPU: " << physicalDevice->getProperties().deviceName << std::endl;
			auto props = physicalDevice->getProperties();
			

			std::cout << "Picking device finished" << std::endl;
		}

		void createLogicalDevice() 
		{

			std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice->getQueueFamilyProperties();
			QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
			float queuePriority = 0.5f;
			std::vector<vk::DeviceQueueCreateInfo> queueInfo;

			//Graphics
			queueInfo.push_back({ {},indices.graphicsFamily,  1,  &queuePriority });
			graphicsFamily = indices.graphicsFamily;
			presentFamily = indices.graphicsFamily;

			//Present queue if graphics and present queues are different
			if (indices.presentFamily != indices.graphicsFamily) {
				queueInfo.push_back(
					vk::DeviceQueueCreateInfo{
						{},
						indices.presentFamily,
						1,
						&queuePriority
					}
				);
				presentFamily = indices.presentFamily;
			}

			vk::StructureChain<
				vk::PhysicalDeviceFeatures2,
				vk::PhysicalDeviceVulkan13Features,
				vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
				vk::PhysicalDeviceVulkan11Features,
				vk::PhysicalDeviceDynamicRenderingFeatures> featureChain;

			// Enable features manually
			featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
			featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;
			featureChain.get<vk::PhysicalDeviceDynamicRenderingFeatures>().dynamicRendering = true;
			featureChain.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy = VK_TRUE;

			std::vector<const char*> deviceExtensions = {
				vk::KHRSwapchainExtensionName,
				vk::KHRSpirv14ExtensionName,
				vk::KHRSynchronization2ExtensionName,
				vk::KHRCreateRenderpass2ExtensionName,
			};

			vk::DeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.queueCreateInfoCount = queueInfo.size();
			deviceCreateInfo.pQueueCreateInfos = queueInfo.data();
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
			deviceCreateInfo.pNext = &featureChain;

			device.emplace(*physicalDevice, deviceCreateInfo);
			graphicsQueue = device->getQueue(indices.graphicsFamily, 0);
			presentQueue = device->getQueue(indices.presentFamily, 0);
			std::cout << "Creating  device finished" << std::endl;
		}

		void createSurface() {
			VkSurfaceKHR       _surface;
			vk::Instance vkInstanceHandle = *instance;

			VkInstance rawInstance = static_cast<VkInstance>(vkInstanceHandle);
			if (glfwCreateWindowSurface(rawInstance, window, nullptr, &_surface) != 0) {
				throw std::runtime_error("failed to create window surface!");
			}
			surface.emplace(*instance, _surface);

			std::cout << "Creating surface finished" << std::endl;
		}

		QueueFamilyIndices findQueueFamilies(std::optional<vk::raii::PhysicalDevice> physicalDevice) {
			QueueFamilyIndices indices{ UINT32_MAX, UINT32_MAX };

			auto queueFamilies = physicalDevice->getQueueFamilyProperties();

			for (uint32_t i = 0; i < queueFamilies.size(); i++) {
				// Graphics support
				if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					indices.graphicsFamily = i;
				}

				// Present support
				if (physicalDevice->getSurfaceSupportKHR(i, *surface)) {
					indices.presentFamily = i;
				}

				if (indices.isComplete()) break;
			}

			return indices;
		}

		void createSwapChain() {
			swapChain.emplace( *device, *physicalDevice, *surface, window,graphicsFamily, presentFamily);
			std::cout << "Creating  swapchain finished" << std::endl;
		}

		void createGraphicsPipeline() {

			
			// Check if the extension VK_KHR_dynamic_rendering is present
			bool dynamicRenderingSupported = false;
			for (auto& ext : physicalDevice->enumerateDeviceExtensionProperties()) {
				if (strcmp(ext.extensionName, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0) {
					dynamicRenderingSupported = true;
					break;
				}
			}

			if (!dynamicRenderingSupported) {
				std::cout << "Dynamic rendering NOT supported!\n";
			}
			else {
				std::cout << "Dynamic rendering supported!\n";
			}


			pipeline.emplace(*descriptor);
			pipeline->createGraphicsPipeline(swapChain, device);

			std::cout << "Creating  pipeline finished" << std::endl;
		}

		void createCommandPool() {
			vk::CommandPoolCreateInfo poolInfo{  vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphicsFamily };
			commandPool.emplace(*device, poolInfo);
			std::cout << "Creating  command pool finished" << std::endl;
		}

		void createCommandBuffer() {

			vk::CommandBufferAllocateInfo allocInfo{  };
			allocInfo.setCommandPool(*commandPool);
			allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
			allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

			commandBuffers.emplace(*device, allocInfo);
			std::cout << "Creating  command buffer finished" << std::endl;
		}

		void recordCommandBuffer(uint32_t imageIndex) {

			commandBuffers->at(currentFrame).begin({});

			transitionImageLayout(
				imageIndex,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eColorAttachmentOptimal,
				{},                                                         // srcAccessMask (no need to wait for previous operations)
				vk::AccessFlagBits2::eColorAttachmentWrite,                 // dstAccessMask
				vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // srcStage
				vk::PipelineStageFlagBits2::eColorAttachmentOutput          // dstStage
			);

			vk::ClearValue clearColor = vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f);
			vk::RenderingAttachmentInfo attachmentInfo;
			attachmentInfo
				.setImageView(swapChain->swapChainImageViews[imageIndex])
				.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.setLoadOp(vk::AttachmentLoadOp::eClear)
				.setStoreOp(vk::AttachmentStoreOp::eStore)
				.setClearValue(clearColor);

			vk::RenderingInfo renderingInfo;
			renderingInfo
				.setRenderArea({ {0, 0}, swapChain->swapChainExtent })
				.setLayerCount(1)
				.setColorAttachmentCount(1)
				.setPColorAttachments(&attachmentInfo);
	
			commandBuffers->at(currentFrame).beginRendering(renderingInfo);

			commandBuffers->at(currentFrame).bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->pipeline.value());

			commandBuffers->at(currentFrame).setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChain->swapChainExtent.width), static_cast<float>(swapChain->swapChainExtent.height), 0.0f, 1.0f));


			

			commandBuffers->at(currentFrame).setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain->swapChainExtent));


			for (auto& sceneObject : scene->sceneObjects) {
				//std::cout << "Object at: " << sceneObject.position.x << ", " << sceneObject.position.y << ", " << sceneObject.position.z << "\n";
				uint16_t meshId = gameObjectManager->gameObjects[gameObjectManager->objectMap[sceneObject.gameObjectType]].meshId;
				uint16_t textureId = gameObjectManager->gameObjects[gameObjectManager->objectMap[sceneObject.gameObjectType]].textureId;
				commandBuffers->at(currentFrame).bindVertexBuffers(0, *(meshManager->getMesh(meshId).vertexBuffer.value()), {0});


				commandBuffers->at(currentFrame).bindIndexBuffer(*(meshManager->getMesh(meshId).indexBuffer), 0, vk::IndexType::eUint16);

				
				glm::mat4 model = sceneObject.getModelMatrix();
				

				commandBuffers->at(currentFrame).pushConstants<glm::mat4>(
					pipeline->pipelineLayout.value(),
					vk::ShaderStageFlagBits::eVertex,
					0,
					model
				);
		

				uint32_t texId = gameObjectManager->gameObjects[gameObjectManager->objectMap[sceneObject.gameObjectType]].textureId;
			

				commandBuffers->at(currentFrame).bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					pipeline->pipelineLayout.value(),
					0,
					*(*descriptor).descriptorSets[texId][currentFrame],
					nullptr
				);

				commandBuffers->at(currentFrame).drawIndexed(meshManager->getMesh(meshId).indexCount, 1, 0, 0, 0);
			}

			commandBuffers->at(currentFrame).endRendering();

			transitionImageLayout(
				imageIndex,
				vk::ImageLayout::eColorAttachmentOptimal,
				vk::ImageLayout::ePresentSrcKHR,
				vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
				{},                                                     // dstAccessMask
				vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
				vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
			);

			commandBuffers->at(currentFrame).end();

		}


		void transitionImageLayout(uint32_t imageIndex,
			vk::ImageLayout oldLayout,
			vk::ImageLayout newLayout,
			vk::AccessFlags2 srcAccessMask,
			vk::AccessFlags2 dstAccessMask,
			vk::PipelineStageFlags2 srcStageMask,
			vk::PipelineStageFlags2 dstStageMask) {
			vk::ImageMemoryBarrier2 barrier;
			barrier
				.setSrcStageMask(srcStageMask)
				.setSrcAccessMask(srcAccessMask)
				.setDstStageMask(dstStageMask)
				.setDstAccessMask(dstAccessMask)
				.setOldLayout(oldLayout)
				.setNewLayout(newLayout)
				.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setImage(swapChain->images[imageIndex])
				.setSubresourceRange(
					vk::ImageSubresourceRange()
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setBaseMipLevel(0)
					.setLevelCount(1)
					.setBaseArrayLayer(0)
					.setLayerCount(1)
				);

			vk::DependencyInfo dependencyInfo;
			dependencyInfo
				.setDependencyFlags({})
				.setImageMemoryBarriers(barrier);

			commandBuffers->at(currentFrame).pipelineBarrier2(dependencyInfo);

		}


		void recreateSwapChain() {
			int width = 0, height = 0;
			glfwGetFramebufferSize(window, &width, &height);
			while (width == 0 || height == 0) {
				glfwGetFramebufferSize(window, &width, &height);
				glfwWaitEvents();
			}
			device->waitIdle();
			swapChain.reset();
			swapChain.emplace(*device, *physicalDevice, *surface, window, graphicsFamily, presentFamily);
		}

		void createBuffers() {

			buffers.emplace(*physicalDevice, *device,*commandPool,*graphicsQueue, MAX_FRAMES_IN_FLIGHT, *swapChain);
		}

		void createDescriptorSetLayout() {
			descriptor = std::make_unique<Descriptor>(*device,*buffers,*textureManager);
		}

		void createDescriptorPool() {
			descriptor->createDescriptorPool(MAX_FRAMES_IN_FLIGHT);
		}

		void createDescriptorSets() {
			descriptor->createDescriptorSets(MAX_FRAMES_IN_FLIGHT,*textureManager,*textureSampler);
		}

		void createTextureManager() {
			std::cout << "Creating Texture and Image..." << std::endl;
			
			textureManager= std::make_unique<TextureManager>(buffers.value(),
				graphicsQueue.value(),
				device.value());
		}

		void createTextureImageSample() {
			std::cout << "Creating Texture Sampler..." << std::endl;
			textureSampler.emplace(*device, *physicalDevice);
		}

		void initGameObjects() {
			gameObjectManager.emplace();
		}
		
		void initMeshManager() {
			meshManager = std::make_unique<MeshManager>(*buffers);
		}

		void initScene() {
			std::cout << "Initializing Scene..." << std::endl;
			scene = std::make_unique<Scene>(*gameObjectManager,*inputManager);
		}

		void createCollisionHandler() {
			collisionHandler.emplace(scoreManager);
		}

		void createInputHandler() {
			inputManager.emplace(window);
		}

		void setupCamera(){
			camera.emplace(*scene);
		}

		void setupProceduralGeneration() {
			proceduralGeneration.emplace(*scene,*camera);
		}


		void initScoreManager() {
			scoreManager = ScoreManager();
		}

};


