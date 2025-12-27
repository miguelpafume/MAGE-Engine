#pragma once

#include <string>
#include <vector>

#include "Device.hpp"

namespace MAGE {

class Pipeline {
public:
	Pipeline(const std::string& vertexFile, const std::string& fragmentFile);

private:
	static std::vector<char> readFile(const std::string& fileName);

	void createGraphicsPipeline(const std::string& vertexFile, const std::string& fragmentFile);
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

} // namespace MAGE