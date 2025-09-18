#include "Renderer.hpp"

namespace MAGE {
Renderer::Renderer(Window &window, Device &device) : window{ window }, device{ device } {
	recreateSwapChain();
	createCommandBuffer();
}

void Renderer::createCommandBuffer() {
	m_commandBuffers.resize(MAGE::SwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFERS!");
	}
}

void Renderer::recreateSwapChain() {
	int width = 0, height = 0;

	window.getFramebufferSize(&width, &height);

	while (width == 0 || height == 0) {
		window.getFramebufferSize(&width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device.getDevice());

	if (swapChain == nullptr) {
		swapChain = std::make_unique<SwapChain>(device);
	} else {

	}

	swapChain->cleanupSwapChain();

	swapChain->createSwapChain();
	swapChain->createImageViews();
	swapChain->createFramebuffers();
}

} // namespace MAGE