#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "Renderer.hpp"

namespace MAGE {

Renderer::Renderer(Window& window, Device& device) : m_window{window}, m_device{device} {
	recreateSwapChain();
	createCommandBuffers();
}

Renderer::~Renderer() {
    freeCommandBuffers();
}

void Renderer::createCommandBuffers() {
	m_commandBuffers.resize(m_swapChain->getImageCount());

	VkCommandBufferAllocateInfo allocInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_device.getCommandPool(),
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size())
	};

	if (vkAllocateCommandBuffers(m_device.getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFERS!");
	}
}

void Renderer::recreateSwapChain() {
	VkExtent2D extent = m_window.getExtent();

	while (extent.width == 0 || extent.height == 0) {
		extent = m_window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device.getDevice());

	if (m_swapChain == nullptr) {
		m_swapChain = std::make_unique<SwapChain>(m_device, extent);
	}
	else {
		std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);
		m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldSwapChain);

		if (!oldSwapChain->compareSwapChainFormats(*m_swapChain.get())) {
			throw std::runtime_error("SWAP CHAIN IMAGE/DEPTH FORMAT HAS CHANGED!");
		}

		if (m_swapChain->getImageCount() != m_commandBuffers.size()) {
			freeCommandBuffers();
			createCommandBuffers();
		}
	}
}

void Renderer::freeCommandBuffers() {
	vkFreeCommandBuffers(m_device.getDevice(), m_device.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	m_commandBuffers.clear();
}

VkCommandBuffer Renderer::beginFrame() {
    assert(!m_isFrameStarted && "CANNOT CALL beginFrame WHILE ALREADY IN PROGRESS!");
	VkResult result = m_swapChain->acquireNextImage(&m_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("FAILED TO ACQUIRE SWAP CHAIN IMAGE!");
	}

    m_isFrameStarted = true;

    VkCommandBuffer commandbuffer = getcurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};

	if (vkBeginCommandBuffer(commandbuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO BEGIN RECORDING COMMAND BUFFER!");
	}

    return commandbuffer;
}

void Renderer::endFrame() {
    assert(m_isFrameStarted && "CANNOT CALL endFrame WHILE FRAME IS NOT IN PROGRESS");
    VkCommandBuffer commandBuffer = getcurrentCommandBuffer();

	if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER!");
	}

    VkResult result = m_swapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.windowResized()) {
		m_window.resetWindowResizedFlag();
		recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO PRESENT SWAP CHAIN IMAGE!");
	}

    m_isFrameStarted = false;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(m_isFrameStarted && "CANNOT CALL beginSwapChainRenderPass IS FRAME IS NOT IN PROGRESS");
    assert(commandBuffer == getcurrentCommandBuffer() && "CANNOT BEGIN RENDER PASS ON COMMAND BUFFER FROM ANOTHER FRAME");

	VkRenderPassBeginInfo renderPassInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = m_swapChain->getRenderPass(),
		.framebuffer = m_swapChain->getFrameBuffer(m_currentImageIndex),
		.renderArea = {
			.offset = {0, 0},
			.extent = m_swapChain->getSwapChainExtent()
		}
	};

	std::array<VkClearValue, 2> clearValues {};
	clearValues[0].color = {0.01f, 0.01f, 0.01f, 0.01f};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(m_swapChain->getSwapChainExtent().width),
		.height = static_cast<float>(m_swapChain->getSwapChainExtent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D scissor({0, 0}, m_swapChain->getSwapChainExtent());
	
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(m_isFrameStarted && "CANNOT CALL endSwapChainRenderPass IS FRAME IS NOT IN PROGRESS");
    assert(commandBuffer == getcurrentCommandBuffer() && "CANNOT END RENDER PASS ON COMMAND BUFFER FROM ANOTHER FRAME");

	vkCmdEndRenderPass(commandBuffer);
}

} // namespace MAGE