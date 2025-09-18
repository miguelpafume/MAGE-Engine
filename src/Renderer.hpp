#pragma once

#include <memory>
#include <vector>

#include "Device.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"

namespace MAGE {
class Renderer {
public:
	Renderer::Renderer(Window& window, Device& device);




private:
	Window& window;
	Device& device;
	std::unique_ptr<SwapChain> swapChain;
	std::vector<VkCommandBuffer> m_commandBuffers;

	
	void recreateSwapChain();
	void createCommandBuffer();
};

} // namespace MAGE