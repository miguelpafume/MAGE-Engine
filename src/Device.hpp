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
#include <iostream>

namespace MAGE {
class Device {
public:
	Device(Window& window);
	~Device();

	VkDevice getDevice() const { return m_device; }
	VkSurfaceKHR getSurface() const { return m_surface; }
	VkInstance getInstance() const { return m_instance; }
	VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
	VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue getPresentQueue() const { return m_presentQueue; }

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	QueueFamilyIndices getQueueFamilies();

private:
	Window& window;

	VkInstance m_instance;
	VkSurfaceKHR m_surface;

	VkDebugUtilsMessengerEXT m_debugMessenger;

	VkDevice m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	void createInstance();
	void createSurface();
	
	void pickPhysicalDevice();
	void createLogicalDevice();

	void setupDebugMessenger();

	bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
	int rateDeviceSuitability(VkPhysicalDevice physicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
};
} // namespace MAGE