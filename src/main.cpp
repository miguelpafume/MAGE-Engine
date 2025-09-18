// GLFW & Vulkan
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// GLM
#include <glm/glm.hpp>

// STD
#include <map>
#include <set>
#include <array>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <optional>
#include <algorithm>
#include <stdexcept>

// CUSTOM
#include "Window.hpp"
#include "Device.hpp"
#include "Mage.hpp"
#include "Renderer.hpp"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindDescription{};
		bindDescription.binding = 0;
		bindDescription.stride = sizeof(Vertex);
		bindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	// POS			// COLOR
	{{0.0f, -0.5f},	{1.0f, 0.0f, 1.0f}},
	{{0.5f, 0.5f},	{1.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f},	{0.0f, 1.0f, 1.0f}}
};

// WINDOW
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// DEBUG
VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

class Application {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	std::unique_ptr<MAGE::Window> window;
	std::unique_ptr<MAGE::Device> device;
	std::unique_ptr<MAGE::Renderer> renderer;

	VkDebugUtilsMessengerEXT debugMessenger;

	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	void initWindow() {
		window = std::make_unique<MAGE::Window>(WIDTH, HEIGHT, "Mage");
	}

	void initVulkan() {
		setupDebugMessenger();
		createDevice();
		swapChain->createSwapChain();
		swapChain->createImageViews();
		swapChain->createRenderPass();
		createGraphicsPipeline();
		swapChain->createFramebuffers();
		createCommandPool();
		createVertexBuffer();
		renderer->createCommandBuffer();
		swapChain->createSyncObjects();
	}

	void createVertexBuffer() {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(vertices[0]) * vertices.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE VERTEX BUFFER!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->getDevice(), vertexBuffer, &memRequirements);
		
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO ALLOCATE VERTEX BUFFER MEMORY!");
		}

		vkBindBufferMemory(device->getDevice(), vertexBuffer, vertexBufferMemory, 0);

		void* data;
		vkMapMemory(device->getDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t) bufferInfo.size);
		vkUnmapMemory(device->getDevice(), vertexBufferMemory);
	}

	void createCommandPool() {
		MAGE::QueueFamilyIndices queueFamilyIndices = device->getQueueFamilies();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE COMMAND POOL!");
		}
	}

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("FAILED TO OPEN FILE!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> content(fileSize);

		file.seekg(0);
		file.read(content.data(), fileSize);

		file.close();

		return content;
	}

	void createGraphicsPipeline() {
		std::vector<char> vertShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/shader.vert.spv");
		std::vector<char> fragShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/shader.frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.pSpecializationInfo = nullptr; // OPTIONAL

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = nullptr; // OPTIONAL

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = dynamicStates.data();

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE; // ### TEMP
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		// OPTIONALS
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicStateInfo;

		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = swapChain->getRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE GRAPHICS PIPELINE!");
		}

		vkDestroyShaderModule(device->getDevice(), vertShaderModule, nullptr);
		vkDestroyShaderModule(device->getDevice(), fragShaderModule, nullptr);
	}

	VkShaderModule createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE SHADER MODULE!");
		}

		return shaderModule;
	}

	void createDevice() {
		device = std::make_unique<MAGE::Device>(window);
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				std::cerr << "[WARNING] USING VK_FORMAT_B8G8R8A8_UNORM INSTEAD OF VK_FORMAT_B8G8R8A8_SRGB" << std::endl;
				return availableFormat;
			}
		}

		std::cerr << "[WARNING] IDEAL SURFACE FORMAT NOT FOUND. USING FIRST FORMAT AVAIABLE: " << availableFormats[0].format << std::endl;
		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes) {
		for (const auto& avaiablePresentMode : avaiablePresentModes) {
			if (avaiablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return avaiablePresentMode;
			}
		}
		
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			uint32_t width = window->getWidth();
			uint32_t height = window->getHeight();

			VkExtent2D actualExtent{ width, height };

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
			return actualExtent;
		}
	};

	void mainLoop() {
		while (!window->shouldClose()) {
			window->poolEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device->getDevice());
	}

	void drawFrame() {
		vkWaitForFences(device->getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device->getDevice(), swapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			swapChain->recreateSwapChain();
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("FAILED ACQUIRE SWAP CHAIN IMAGE!");
		}

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(device->getDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		vkResetFences(device->getDevice(), 1, &inFlightFences[currentFrame]);
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex] };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->wasResized()) {
			recreateSwapChain();
			window->resetResizeFlag();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO PRESENT SWAP CHAIN IMAGE!");
		}

		currentFrame = (currentFrame + 1) % MAGE::SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void cleanup() {
		if (enableValidationLayers)
			destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		swapChain->cleanupSwapChain();

		vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device->getDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device->getDevice(), inFlightFences[i], nullptr);
		}

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroySemaphore(device->getDevice(), renderFinishedSemaphores[i], nullptr);
		}

		vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);

		vkDestroyPipeline(device->getDevice(), graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, nullptr);

		vkDestroyRenderPass(device->getDevice(), renderPass, nullptr);

		device->cleanup();
		
		glfwTerminate();
	}

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("FAILES TO SET UP DEBUG MESSENGER!");
		}
	}
};

int main() {
	Application app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}