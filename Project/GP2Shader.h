#pragma once
#include <vulkan/vulkan_core.h>
#include <vulkanbase/VulkanUtil.h>
#include <vector>
#include "Vertex.h"
#include "Buffer.h"
struct Vertex;
class GP2Shader final
{
public:
	GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) 
		: m_VertexShaderFile{ vertexShaderFile }
		, m_FragmentShaderFile{ fragmentShaderFile }
		, m_ShaderStages{}
		, m_InputBinding{ Vertex::GetBindingDescription() }
		, m_AttributeDescriptions{ Vertex::GetAttributeDescriptions() }
	{

	}

	~GP2Shader() = default;
	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() { return m_ShaderStages; }

	void initialize(const VkPhysicalDevice& vkPhysicalDevice, const VkDevice& vkDevice);
	void destroyShaderModules(const VkDevice& vkDevice);
	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();

	// UBO binding
	void createDescriptorSetLayout(const VkDevice& vkDevice);
	void bindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);
	const VkDescriptorSetLayout& getDescriptorSetLayout() const{ return m_DescriptorSetLayout; } 
private:
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice);
	VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code);

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions;
	VkVertexInputBindingDescription m_InputBinding;

	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::unique_ptr<Buffer> m_UBOBuffer;
	VertexUBO m_UBOSrc;
	//std::unique_ptr<DescriptorPool> m_DescriptorPool;

	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator= (const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator= (const GP2Shader&&) = delete;
};
