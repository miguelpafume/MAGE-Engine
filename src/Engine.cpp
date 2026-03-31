#include <memory>
#include <stdexcept>
#include <array>

#include <vulkan/vulkan_core.h>

#include "Engine.hpp"

namespace MAGE {

Engine::Engine() { 
	loadModel();
	createPipelineLayout();
	createPipeline();
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

void Engine::triangleFractal(
	std::vector<Model::Vertex> &vertices,
	int depth,
	glm::vec2 left,
	glm::vec2 right,
	glm::vec2 top) {

	if (depth <= 0) {
		vertices.push_back({top});
		vertices.push_back({right});
		vertices.push_back({left});
	} else {
		auto leftTop = 0.5f * (left + top);
		auto rightTop = 0.5f * (right + top);
		auto leftRight = 0.5f * (left + right);

		triangleFractal(vertices, depth - 1, left, leftRight, leftTop);
		triangleFractal(vertices, depth - 1, leftRight, right, rightTop);
		triangleFractal(vertices, depth - 1, leftTop, rightTop, top);
	}
}

void Engine::loadModel() {
	// std::vector<Model::Vertex> vertices {
	// 	{{0.0f, -0.5f}},
	// 	{{0.5f, 0.5f}},
	// 	{{-0.5f, 0.5f}}
	// };

	std::vector<Model::Vertex> vertices {};

	triangleFractal(vertices, 5, {-0.7f, 0.7f}, {0.7f, 0.7f}, {0.0f, -0.7f});

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
	Pipeline::defaultPipelineConfigInfo(m_swapChain.getWidth(), m_swapChain.getHeight(), pipelineConfig);
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	pipelineConfig.renderPass = m_swapChain.getRenderPass();

	m_pipeline = std::make_unique<Pipeline>(
		m_device,
		pipelineConfig,
		"simpleVert.vert.slang.spv",
		"simpleFrag.frag.slang.spv"
	);
}

void Engine::createCommandBuffers() {
	m_commandBuffers.resize(m_swapChain.getImageCount());

	VkCommandBufferAllocateInfo allocInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_device.getCommandPool(),
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size())
	};

	if (vkAllocateCommandBuffers(m_device.getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFERS!");
	}

	for (int i = 0; i < m_commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};

		if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO BEGIN RECORDING COMMAND BUFFER!");
		}

		VkRenderPassBeginInfo renderPassInfo {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = m_swapChain.getRenderPass(),
			.framebuffer = m_swapChain.getFrameBuffer(i),
			.renderArea = {
				.offset = {0, 0},
				.extent = m_swapChain.getSwapChainExtent()
			}
		};

		std::array<VkClearValue, 2> clearValues {};
		clearValues[0].color = {0.01f, 0.01f, 0.01f, 0.01f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_pipeline->bind(m_commandBuffers[i]);
		m_model->bind(m_commandBuffers[i]);
		m_model->draw(m_commandBuffers[i]);

		vkCmdEndRenderPass(m_commandBuffers[i]);

		if(vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
		}
	}
}

void Engine::drawFrame() {
	uint32_t imageIndex;
	VkResult result = m_swapChain.acquireNextImage(&imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("FAILED TO ACQUIRE SWAP CHAIN IMAGE!"); //### CAN OCCUR WHEN A WINDOW RESIZES?
	}

	result = m_swapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO PRESENT SWAP CHAIN IMAGE!");
	}
}

} // namespace MAGE