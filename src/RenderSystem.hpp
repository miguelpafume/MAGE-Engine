#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "Model.hpp"
#include "GameObject.hpp"
#include "Util.hpp"

namespace MAGE {

class RenderSystem {
public:
	RenderSystem(Device& device, VkRenderPass renderPass);
	~RenderSystem();

	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
    
    void renderGameObject(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, float deltaTime);
	
private:
	void createPipeline(VkRenderPass renderPass);
	void createPipelineLayout();

	Device& m_device;

	std::unique_ptr<Pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;
};

} // namespace MAGE