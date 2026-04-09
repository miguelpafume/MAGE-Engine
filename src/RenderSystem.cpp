#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "RenderSystem.hpp"

namespace MAGE {

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) : m_device{device} { 
	createPipelineLayout();
	createPipeline(renderPass);
}

RenderSystem::~RenderSystem() { 
	vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout() {
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

void RenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(m_pipelineLayout != nullptr && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT!");

	PipelineConfigInfo pipelineConfig;
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	pipelineConfig.renderPass = renderPass;

	m_pipeline = std::make_unique<Pipeline>(
		m_device,
		pipelineConfig,
		"simpleVert.vert.slang.spv",
		"simpleFrag.frag.slang.spv"
	);
}

void RenderSystem::renderGameObject(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera) {
	m_pipeline->bind(commandBuffer);

	glm::mat4x4 projectionView = camera.getProjection() * camera.getView();

	for (GameObject& obj: gameObjects) {
		glm::mat4x4 pushTransform = projectionView * obj.m_transform.mat4x4();

		SimplePushConstantData push{
			.transform = pushTransform,
			.color = obj.m_color
		};

		vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		obj.m_model->bind(commandBuffer);
		obj.m_model->draw(commandBuffer);
	}
}

} // namespace MAGE