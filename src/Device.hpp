#pragma once

#include <vulkan/vulkan.h>

#include <map>
#include <set>
#include <vector>
#include <stdexcept>

#include "Mage.hpp"
#include "SwapChain.hpp"

#include <assert.h>

namespace MAGE {
class Device {
public:
	Device(VkInstance instance, VkSurfaceKHR surface);
	void cleanup();

	VkDevice getDevice() const { return m_device; }
	VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
	VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue getPresentQueue() const { return m_presentQueue; }

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	QueueFamilyIndices getQueueFamilies();

private:
	VkInstance m_instance;
	VkSurfaceKHR m_surface;

	VkDevice m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

private:
	void pickPhysicalDevice();
	void createLogicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
	int rateDeviceSuitability(VkPhysicalDevice physicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
};
} // namespace MAGE