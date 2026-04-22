#pragma once

#include <vector>

#include "Window.hpp"
#include "Util.hpp"

namespace MAGE {

class Device {
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	Device(Window& window);
	~Device();

	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }
	QueueFamilyIndices findPhysicalQueueFamilies() { return getQueueFamilies(m_physicalDevice); }
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
	
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1);
	void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	
	VkPhysicalDeviceProperties properties;

	VkCommandPool getCommandPool() 	const { return m_commandPool; }
	VkDevice getDevice() 			const { return m_device; }
	VkSurfaceKHR getSurface() 		const { return m_surface; }
	VkQueue getGraphicsQueue() 		const { return m_graphicsQueue; }
	VkQueue getPresentQueue() 		const { return m_presentQueue; }

private:
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();

	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice physicalDevice);

	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void hasGflwRequiredInstanceExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	Window& window;
	
	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	
	VkDevice m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkCommandPool m_commandPool;
	
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	
	VkDebugUtilsMessengerEXT m_debugMessenger;
	
	const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

} // namespace MAGE
