#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "Engine.hpp"

namespace MAGE {

Engine::Engine() { 
	loadModel();
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

struct TriangleFracState {
	int depth;
	glm::vec2 left, right, top;
};

void Engine::triangleFractal(
	std::vector<Model::Vertex> &vertices,
	int depth,
	glm::vec2 left,
	glm::vec2 right,
	glm::vec2 top) {

	std::queue<TriangleFracState> queue;
	queue.push({depth, left, right, top});

	while (!queue.empty()) {
		TriangleFracState currentTriangle = queue.front();
		queue.pop();

		if (currentTriangle.depth <= 0) {
			vertices.push_back({currentTriangle.top});
			vertices.push_back({currentTriangle.right});
			vertices.push_back({currentTriangle.left});
		} else {
			glm::vec2 leftTop = 0.5f * (currentTriangle.left + currentTriangle.top);
			glm::vec2 rightTop = 0.5f * (currentTriangle.right + currentTriangle.top);
			glm::vec2 leftRight = 0.5f * (currentTriangle.left + currentTriangle.right);

			queue.push({currentTriangle.depth - 1, currentTriangle.left, leftRight, leftTop});
			queue.push({currentTriangle.depth - 1, leftRight, currentTriangle.right, rightTop});
			queue.push({currentTriangle.depth - 1, leftTop, rightTop, currentTriangle.top});
		}
	}
}

void Engine::loadModel() {
	std::vector<Model::Vertex> vertices {
		{{ 0.0f, -0.7f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.7f,  0.7f}, {0.0f, 1.0f, 0.0f}},
		{{-0.7f,  0.7f}, {0.0f, 0.0f, 1.0f}}
	};

	// std::vector<Model::Vertex> vertices {};
	// triangleFractal(vertices, 5, {-0.7f, 0.7f}, {0.7f, 0.7f}, {0.0f, -0.7f});

	m_model = std::make_unique<Model>(m_device, vertices);
}

void Engine::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};

	if (vkCreatePipelineLayout(m_device.getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT!");
	}
}

void Engine::createPipeline() {
	PipelineConfigInfo pipelineConfig;
	Pipeline::defaultPipelineConfigInfo(m_swapChain->getWidth(), m_swapChain->getHeight(), pipelineConfig);
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

	m_pipeline->bind(m_commandBuffers[imageIndex]);
	m_model->bind(m_commandBuffers[imageIndex]);
	m_model->draw(m_commandBuffers[imageIndex]);

	vkCmdEndRenderPass(m_commandBuffers[imageIndex]);

	if(vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
	}
}

void Engine::recreateSwapChain() {
	VkExtent2D extent = m_window.getExtent();

	while (extent.width == 0 || extent.height == 0) {
		extent = m_window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device.getDevice());

	m_swapChain = std::make_unique<SwapChain>(m_device, extent);
	createPipeline();
}

void Engine::drawFrame() {
	uint32_t imageIndex;
	VkResult result = m_swapChain->acquireNextImage(&imageIndex);

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