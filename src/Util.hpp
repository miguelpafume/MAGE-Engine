#pragma once

#include <vector>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace MAGE {

struct SimplePushConstantData {
	glm::mat2x2 transform{1.f};
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;

	static std::vector<VkVertexInputBindingDescription> getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
	bool graphicsFamilyHasValue = false;
	bool presentFamilyHasValue = false;
	bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

} // namespace MAGE