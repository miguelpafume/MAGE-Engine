#include <memory>
#include <stdexcept>

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
	}
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

}

void Engine::drawFrame() {

}

} // namespace MAGE