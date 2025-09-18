#include <vulkan/vulkan.h>

#include "Mage.hpp"
#include "Device.hpp"

namespace MAGE {
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface); //###surface is temporary

class SwapChain {
public:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	SwapChain::SwapChain(MAGE::Device &device);

	void cleanupSwapChain();
	void createSyncObjects();
	void createFramebuffers();
	void createRenderPass();
	void createImageViews();
	void createSwapChain();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
	std::vector<VkFramebuffer> getSwapChainFramebuffers() { return m_swapChainFramebuffers; }
	VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
	VkRenderPass getRenderPass() { return m_renderPass; }

private:
	MAGE::Device &device;

	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	uint32_t m_currentFrame = 0;

	VkRenderPass m_renderPass;
};



} // namespace MAGE