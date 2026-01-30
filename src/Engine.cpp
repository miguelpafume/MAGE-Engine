#include <memory>
#include <stdexcept>
#include <array>

#include <vulkan/vulkan_core.h>

#include "Engine.hpp"

namespace MAGE {

Engine::Engine() { 
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
		"simple_shader.vert.spv",
		"simple_shader.frag.spv"
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
		vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(m_commandBuffers[i]);

		if(vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
		}
	}
}

void Engine::drawFrame() {
	uint32_t imageIndex;
	VkResult result = m_swapChain.acquireNextImage(&imageIndex);
	std::cout << std::endl;
	std::cout << imageIndex << std::endl << std::endl;

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("FAILED TO ACQUIRE SWAP CHAIN IMAGE!"); //### CAN OCCUR WHEN A WINDOW RESIZES?
	}

	result = m_swapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO PRESENT SWAP CHAIN IMAGE!");
	}
}

} // namespace MAGE