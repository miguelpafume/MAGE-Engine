#pragma once

#include <memory>
#include <vector>

#include "Window.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"

namespace MAGE {

class Engine {
public:
	void run();

	Engine() {};
	~Engine() {};

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	void createPipeline();
	void createPipelineLayout();
	void createCommandBuffers();
	void drawFrame();

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
	Device m_device{m_window};
	SwapChain m_swapChain{m_device, m_window.getExtent()};
	
	std::unique_ptr<Pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	std::vector<VkCommandBuffer> m_commandBuffers;

};

} // namespace MAGE