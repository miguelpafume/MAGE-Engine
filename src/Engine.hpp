#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

#include "Window.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "Model.hpp"
#include "GameObject.hpp"

namespace MAGE {

class Engine {
public:
	void triangleFractal(std::vector<Model::Vertex> &vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);

	void run();

	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	void loadGameObjects();
	void createPipeline();
	void createPipelineLayout();
	void createCommandBuffers();
	void drawFrame();
	void recreateSwapChain();
	void recordCommandBuffer(int imageIndex);
	void freeCommandBuffers();
	void renderGameObject(VkCommandBuffer commandBuffer);

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
	Device m_device{m_window};
	std::unique_ptr<SwapChain> m_swapChain;
	
	std::unique_ptr<Pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	std::vector<VkCommandBuffer> m_commandBuffers;

	std::vector<GameObject> m_gameObjects;
};

} // namespace MAGE