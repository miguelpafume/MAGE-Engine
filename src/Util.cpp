#include "Util.hpp"

namespace MAGE {

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescription() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

    bindingDescriptions[0] = {
        .binding = 0,   
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

    // Position
    attributeDescriptions[0] = {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, position)
    };

    // Color
    attributeDescriptions[1] = {
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color)
    };
    
    // Texture Coordinates
    attributeDescriptions[2] = {
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, texCoord)
    };

    return attributeDescriptions;
}

std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("FAILED TO OPEN FILE: " + filename);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> content(fileSize);

	file.seekg(0);
	file.read(content.data(), fileSize);

	file.close();

	return content;
}

} // namespace MAGE