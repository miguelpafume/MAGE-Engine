#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "RenderSystem.hpp"

namespace MAGE {

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout) : m_device{device} { 
	createPipelineLayout(globalSetLayout, textureSetLayout);
	createPipeline(renderPass);
}

RenderSystem::~RenderSystem() { 
	vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout) {
	VkPushConstantRange pushConstantRange{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(SimplePushConstantData),
	};
	
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout, textureSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
		.pSetLayouts = descriptorSetLayouts.data(),
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

void RenderSystem::renderGameObject(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects) {
	m_pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	for (GameObject& obj: gameObjects) {
		if (obj.m_textureDescriptorSet != VK_NULL_HANDLE) {
			vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 1, 1, &obj.m_textureDescriptorSet, 0, nullptr);
		}

		SimplePushConstantData push {
			.modelMatrix = obj.m_transform.mat4x4(),
			.color = obj.m_color
		};

		vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		obj.m_model->bind(frameInfo.commandBuffer);
		obj.m_model->draw(frameInfo.commandBuffer);
	}
}

} // namespace MAGE