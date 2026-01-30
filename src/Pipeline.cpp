#include "Pipeline.hpp"

namespace MAGE {
Pipeline::Pipeline(Device &device, const PipelineConfigInfo &configInfo, const std::string &vertexFile, const std::string &fragmentFile) : m_device(device){
	createGraphicsPipeline(configInfo, vertexFile, fragmentFile);
}

Pipeline::~Pipeline() {
	vkDestroyShaderModule(m_device.getDevice(), m_vertexShaderModule, nullptr);
	vkDestroyShaderModule(m_device.getDevice(), m_fragmentShaderModule, nullptr);
	vkDestroyPipeline(m_device.getDevice(), m_graphicsPipeline, nullptr);
}

void Pipeline::createGraphicsPipeline(const PipelineConfigInfo &configInfo, const std::string& vertexFile, const std::string& fragmentFile) {
	assert(configInfo.pipelineLayout != nullptr && "Cannot create graphics pipeline: no pipeline layout provided in config info");
	assert(configInfo.renderPass != nullptr && "Cannot create graphics pipeline: no render pass provided in config info");
	
	std::vector<char> vertShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/" + vertexFile);
	std::vector<char> fragShaderCode = readFile(std::string(SOURCE_DIR) + "/shaders/" + fragmentFile);

	createShaderModule(vertShaderCode, &m_vertexShaderModule);
	createShaderModule(fragShaderCode, &m_fragmentShaderModule);

	VkPipelineShaderStageCreateInfo shaderStages[2];
	// Vertex Shader Stage
	shaderStages[0] = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = m_vertexShaderModule,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};
	
	// Fragment shader stage
	shaderStages[1] = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = m_fragmentShaderModule,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr
	};

	VkPipelineViewportStateCreateInfo viewportInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &configInfo.viewport,
		.scissorCount = 1,
		.pScissors = &configInfo.scissor
	};

	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &configInfo.inputAssemblyInfo,
		.pViewportState = &viewportInfo,
		.pRasterizationState = &configInfo.rasterizationInfo,
		.pMultisampleState = &configInfo.multisampleInfo,
		.pDepthStencilState = &configInfo.depthStencilInfo,
		.pColorBlendState = &configInfo.colorBlendInfo,
		.pDynamicState = nullptr,

		.layout = configInfo.pipelineLayout,
		.renderPass = configInfo.renderPass,
		.subpass = configInfo.subpass,

		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	if (vkCreateGraphicsPipelines(m_device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO CREATE GRAPHICS PIPELINE!");
	}
}

void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule *shaderModule) {
	VkShaderModuleCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	};

	if(vkCreateShaderModule(m_device.getDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO CREATE SHADER MODULE!");
	}
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
}

void Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height, PipelineConfigInfo &configInfo)
{
    configInfo.inputAssemblyInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	configInfo.scissor = {
		.offset = {0, 0},
		.extent = {width, height}
	};

	configInfo.viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(width),
		.height = static_cast<float>(height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	configInfo.rasterizationInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f
	};

	configInfo.multisampleInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};

	configInfo.colorBlendAttachment = {
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask =	VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
							VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	configInfo.colorBlendInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &configInfo.colorBlendAttachment
	};
	configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
	configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

	configInfo.depthStencilInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f
	};
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