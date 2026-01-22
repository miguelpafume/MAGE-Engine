#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "Device.hpp"

namespace MAGE {

struct PipelineConfigInfo {
};

class Pipeline {
public:
	Pipeline(
		Device &device,
		const PipelineConfigInfo &configInfo,
		const std::string &vertexFile,
		const std::string &fragmentFile);

	~Pipeline() {};

	Pipeline(const Pipeline&) = delete;
	void operator=(const Pipeline&) = delete;
	
	static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
	static std::vector<char> readFile(const std::string &fileName);

	void createGraphicsPipeline(
		const PipelineConfigInfo &configInfo,
		const std::string &vertexFile,
		const std::string &fragmentFile);
	
	void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

	Device &m_device;
	VkPipeline m_graphicsPipeline;
	VkShaderModule m_vertexShaderModule;
	VkShaderModule m_fragmentShaderModule;
};

} // namespace MAGE