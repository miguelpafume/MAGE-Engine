#include "Pipeline.hpp"

namespace MAGE {
Pipeline::Pipeline(const std::string& vertexFile, const std::string& fragmentFile) {
	createGraphicsPipeline(vertexFile, fragmentFile);
}

void Pipeline::createGraphicsPipeline(const std::string& vertexFile, const std::string& fragmentFile) {
	std::vector<char> vertShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/" + vertexFile);
	std::vector<char> fragShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/" + fragmentFile);


}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule = nullptr;
	if(vkCreateShaderModule(Device))
}

std::vector<char> Pipeline::readFile(const std::string& filename) {
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