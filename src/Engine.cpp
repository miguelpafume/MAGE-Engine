#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "Engine.hpp"

namespace MAGE {

// ###TEMP
struct SimplePushConstantData {
	glm::mat2x2 transform{1.f};
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Engine::Engine() { 
	loadGameObjects();
	createPipelineLayout();
	recreateSwapChain();
	createCommandBuffers();
}

Engine::~Engine() { 
	vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
}

void Engine::run() {
	while (!m_window.shouldClose()) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(m_device.getDevice());
}

// struct TriangleFracState {
// 	int depth;
// 	glm::vec2 left, right, top;
// };

// void Engine::triangleFractal(
// 	std::vector<Model::Vertex> &vertices,
// 	int depth,
// 	glm::vec2 left,
// 	glm::vec2 right,
// 	glm::vec2 top) {

// 	std::queue<TriangleFracState> queue;
// 	queue.push({depth, left, right, top});

// 	while (!queue.empty()) {
// 		TriangleFracState currentTriangle = queue.front();
// 		queue.pop();

// 		if (currentTriangle.depth <= 0) {
// 			vertices.push_back({currentTriangle.top});
// 			vertices.push_back({currentTriangle.right});
// 			vertices.push_back({currentTriangle.left});
// 		} else {
// 			glm::vec2 leftTop = 0.5f * (currentTriangle.left + currentTriangle.top);
// 			glm::vec2 rightTop = 0.5f * (currentTriangle.right + currentTriangle.top);
// 			glm::vec2 leftRight = 0.5f * (currentTriangle.left + currentTriangle.right);

// 			queue.push({currentTriangle.depth - 1, currentTriangle.left, leftRight, leftTop});
// 			queue.push({currentTriangle.depth - 1, leftRight, currentTriangle.right, rightTop});
// 			queue.push({currentTriangle.depth - 1, leftTop, rightTop, currentTriangle.top});
// 		}
// 	}
// }

void Engine::loadGameObjects() {
	std::vector<Model::Vertex> vertices {
		{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	// std::vector<Model::Vertex> vertices {};
	// triangleFractal(vertices, 5, {-0.7f, 0.7f}, {0.7f, 0.7f}, {0.0f, -0.7f});

	std::shared_ptr<Model> m_model = std::make_shared<Model>(m_device, vertices);

	GameObject triangle = GameObject::createGameObject();
	triangle.m_model = m_model;
	triangle.m_color = {0.1f, 0.8f, 0.1f};
	triangle.m_transform2d.translation.x = 0.2f;
	triangle.m_transform2d.scale = {2.0f, 0.5f};
	triangle.m_transform2d.rotation = 0.25f * glm::two_pi<float>();

	m_gameObjects.push_back(std::move(triangle));
}

void Engine::createPipelineLayout() {
	VkPushConstantRange pushConstantRange{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(SimplePushConstantData),
	};
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};

	if (vkCreatePipelineLayout(m_device.getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT!");
	}
}

void Engine::createPipeline() {
	assert(m_swapChain != nullptr && "CANNOT CREATE PIPELINE BEFORE SWAP CHAIN!");
	assert(m_pipelineLayout != nullptr && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT!");

	PipelineConfigInfo pipelineConfig;
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	pipelineConfig.renderPass = m_swapChain->getRenderPass();

	m_pipeline = std::make_unique<Pipeline>(
		m_device,
		pipelineConfig,
		"simpleVert.vert.slang.spv",
		"simpleFrag.frag.slang.spv"
	);
}

void Engine::createCommandBuffers() {
	m_commandBuffers.resize(m_swapChain->getImageCount());

	VkCommandBufferAllocateInfo allocInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_device.getCommandPool(),
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size())
	};

	if (vkAllocateCommandBuffers(m_device.getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFERS!");
	}
}

void Engine::recordCommandBuffer(int imageIndex) {
	VkCommandBufferBeginInfo beginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};

	if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO BEGIN RECORDING COMMAND BUFFER!");
	}

	VkRenderPassBeginInfo renderPassInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = m_swapChain->getRenderPass(),
		.framebuffer = m_swapChain->getFrameBuffer(imageIndex),
		.renderArea = {
			.offset = {0, 0},
			.extent = m_swapChain->getSwapChainExtent()
		}
	};

	std::array<VkClearValue, 2> clearValues {};
	clearValues[0].color = {0.01f, 0.01f, 0.01f, 0.01f};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(m_swapChain->getSwapChainExtent().width),
		.height = static_cast<float>(m_swapChain->getSwapChainExtent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D scissor({0, 0}, m_swapChain->getSwapChainExtent());
	
	vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissor);

	renderGameObject(m_commandBuffers[imageIndex]);

	vkCmdEndRenderPass(m_commandBuffers[imageIndex]);

	if(vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
	}
}

void Engine::renderGameObject(VkCommandBuffer commandBuffer) {
	m_pipeline->bind(commandBuffer);

	for (GameObject& obj: m_gameObjects) {
		obj.m_transform2d.rotation = glm::mod(obj.m_transform2d.rotation + 0.5f * m_deltaTime, glm::two_pi<float>());

		SimplePushConstantData push{
			.transform = obj.m_transform2d.mat2x2(),
			.offset = obj.m_transform2d.translation,
			.color = obj.m_color
		};

		vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		obj.m_model->bind(commandBuffer);
		obj.m_model->draw(commandBuffer);
	}
}

void Engine::recreateSwapChain() {
	VkExtent2D extent = m_window.getExtent();

	while (extent.width == 0 || extent.height == 0) {
		extent = m_window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device.getDevice());

	if (m_swapChain == nullptr) {
		m_swapChain = std::make_unique<SwapChain>(m_device, extent);
	}
	else {
		m_swapChain = std::make_unique<SwapChain>(m_device, extent, std::move(m_swapChain));
		if (m_swapChain->getImageCount() != m_commandBuffers.size()) {
			freeCommandBuffers();
			createCommandBuffers();
		}
	}

	createPipeline();
}

void Engine::freeCommandBuffers() {
	vkFreeCommandBuffers(m_device.getDevice(), m_device.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	m_commandBuffers.clear();
}

void Engine::drawFrame() {
	uint32_t imageIndex;
	VkResult result = m_swapChain->acquireNextImage(&imageIndex);

	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("FAILED TO ACQUIRE SWAP CHAIN IMAGE!");
	}

	recordCommandBuffer(imageIndex);
	result = m_swapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window	.windowResized()) {
		m_window.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO PRESENT SWAP CHAIN IMAGE!");
	}
}

} // namespace MAGE