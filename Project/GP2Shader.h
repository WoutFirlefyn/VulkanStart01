#pragma once
#include <vulkan/vulkan_core.h>
#include <vulkanbase/VulkanUtil.h>
#include <vector>
#include "Vertex.h"
#include "Buffer.h"
#include "DescriptorPool.h"
struct Vertex;
class GP2Shader final
{
public:
	GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

	~GP2Shader();

	void initialize(const VulkanContext& context);
	void destroyShaderModules(const VkDevice& vkDevice);
	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() { return m_ShaderStages; }
private:
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice);
	VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code);

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator= (const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator= (const GP2Shader&&) = delete;
};
