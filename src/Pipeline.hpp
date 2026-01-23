#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cassert>

#include "Device.hpp"

namespace MAGE {

struct PipelineConfigInfo {
	VkRect2D scissor;
	VkViewport viewport;
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class Pipeline {
public:
	Pipeline(
		Device &device,
		const PipelineConfigInfo &configInfo,
		const std::string &vertexFile,
		const std::string &fragmentFile);

	~Pipeline();

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