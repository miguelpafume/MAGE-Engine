#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MAGE {

struct TransformComponent {
    glm::vec3 translation {}; // Position offset
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation {};

    glm::mat4x4 mat4x4() { 
		// X Y Z tait-bryan angle
		// const float c1 = glm::cos(rotation.x);
		// const float c2 = glm::cos(rotation.y);
		// const float c3 = glm::cos(rotation.z);

		// const float s1 = glm::sin(rotation.x);
		// const float s2 = glm::sin(rotation.y);
		// const float s3 = glm::sin(rotation.z);

		// glm::mat4x4 transform = glm::mat4x4 {
		// 	{
		// 		scale.x * (c2 * c3),
		// 		scale.x * (c1 * s3 + c3 * s1 * s2),
		// 		scale.x * (s1 * s3 - c1 * c3 * s2),
		// 		0.0f
		// 	},
		// 	{
		// 		scale.y * (-c2 * s3),
		// 		scale.y * (c1 * c3 - s1 * s2 * s3),
		// 		scale.y * (c3 * s1 + c1 * s2 * s3),
		// 		0.0f
		// 	},
		// 	{
		// 		scale.z * (s2),
		// 		scale.z * (-c2 * s1),
		// 		scale.z * (c1 * c2),
		// 		0.0f
		// 	},
		// 	{
		// 		translation.x,
		// 		translation.y,
		// 		translation.z,
		// 		1.0f 
		// 	}
		// };

		// Y X Z tait-bryan angle
		const float c1 = glm::cos(rotation.y);
		const float c2 = glm::cos(rotation.x);
		const float c3 = glm::cos(rotation.z);

		const float s1 = glm::sin(rotation.y);
		const float s2 = glm::sin(rotation.x);
		const float s3 = glm::sin(rotation.z);

		glm::mat4x4 transform = glm::mat4x4 {
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f
			},
			{
				translation.x,
				translation.y,
				translation.z,
				1.0f 
			}
		};

		return transform;
    }
};

struct SimplePushConstantData {
	glm::mat4x4 transform{1.f};
	alignas(16) glm::vec3 color;
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;

	static std::vector<VkVertexInputBindingDescription> getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

struct Builder {
	std::vector<Vertex> vertices {};
	std::vector<uint32_t> indices {};
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

std::vector<char> readFile(const std::string& filename);

} // namespace MAGE