#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cassert>

#include "Device.hpp"
#include "Model.hpp"
#include "Util.hpp"

namespace MAGE {

struct PipelineConfigInfo {
	PipelineConfigInfo() = default;

	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo 		viewportInfo{};
	VkPipelineInputAssemblyStateCreateInfo 	inputAssemblyInfo {};
	VkPipelineRasterizationStateCreateInfo 	rasterizationInfo {};
	VkPipelineMultisampleStateCreateInfo 	multisampleInfo {};
	VkPipelineColorBlendAttachmentState 	colorBlendAttachment {};
	VkPipelineColorBlendStateCreateInfo 	colorBlendInfo {};
	VkPipelineDepthStencilStateCreateInfo 	depthStencilInfo {};
	VkPipelineDynamicStateCreateInfo		dynamicStateInfo{};
	std::vector<VkDynamicState> dynamicStateEnabled;
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
	Pipeline& operator=(const Pipeline&) = delete;
	
	void bind(VkCommandBuffer commandBuffer);
	static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

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