#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "GP2Shader.h"
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "DescriptorPool.h"

GP2Shader::GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	: m_VertexShaderFile{ vertexShaderFile }
	, m_FragmentShaderFile{ fragmentShaderFile }
	, m_ShaderStages{}
{
}

GP2Shader::~GP2Shader() = default;

void GP2Shader::initialize(const VulkanContext& context)
{
	m_ShaderStages.push_back(createFragmentShaderInfo(context.device));
	m_ShaderStages.push_back(createVertexShaderInfo(context.device));
}

void GP2Shader::destroyShaderModules(const VkDevice& vkDevice)
{
	for (VkPipelineShaderStageCreateInfo& stageInfo : m_ShaderStages)
		vkDestroyShaderModule(vkDevice, stageInfo.module, nullptr);
	m_ShaderStages.clear();
}

VkPipelineShaderStageCreateInfo GP2Shader::createFragmentShaderInfo(const VkDevice& vkDevice) 
{
	std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = createShaderModule(vkDevice, fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo GP2Shader::createVertexShaderInfo(const VkDevice& vkDevice) 
{
	std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = createShaderModule(vkDevice, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

VkShaderModule GP2Shader::createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code) 
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}