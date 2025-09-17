#pragma once

#include <vulkan/vulkan.h>

#include <map>
#include <set>
#include <vector>
#include <stdexcept>

#include "Mage.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"

#include <assert.h>

namespace MAGE {
class Device {
public:
	Device(MAGE::Window &window);
	void cleanup();

	void createSurface();
	void createInstance();

	VkDevice getDevice() const { return m_device; }
	VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
	VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue getPresentQueue() const { return m_presentQueue; }
	VkInstance getInstance() const { return m_instance; }
	VkSurfaceKHR getSurface() const { return m_surface; }

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	QueueFamilyIndices getQueueFamilies();

private:
	MAGE::Window &window;

	VkInstance m_instance;
	VkSurfaceKHR m_surface;

	VkDevice m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

private:
	void pickPhysicalDevice();
	void createLogicalDevice();

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
	int rateDeviceSuitability(VkPhysicalDevice physicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
};
} // namespace MAGE