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
	//, m_InputBinding{ Vertex::GetBindingDescription() }
	//, m_AttributeDescriptions{ Vertex::GetAttributeDescriptions() }
	//, m_DescriptorSetLayout{}
	, m_UBOSrc{}
{
}

GP2Shader::~GP2Shader() = default;

void GP2Shader::initialize(const VkPhysicalDevice& vkPhysicalDevice, const VkDevice& vkDevice)
{
	m_ShaderStages.push_back(createFragmentShaderInfo(vkDevice));
	m_ShaderStages.push_back(createVertexShaderInfo(vkDevice));

	m_UBOBuffer = std::make_unique<Buffer>(
		vkPhysicalDevice,
		vkDevice,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(VertexUBO)
	);

	m_UBOBuffer->Map();
	//m_DescriptorPool = std::make_unique<DescriptorPool>(vkDevice, m_UBOBuffer->GetSizeInBytes(), 1);
}

void GP2Shader::initialize(const VulkanContext& context)
{
	initialize(context.physicalDevice, context.device);
}

void GP2Shader::destroy(const VkDevice& vkDevice)
{
	//vkDestroyDescriptorSetLayout(vkDevice, m_DescriptorSetLayout, nullptr);

	m_UBOBuffer.reset();
	//m_DescriptorPool.reset();
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

//VkPipelineVertexInputStateCreateInfo GP2Shader::createVertexInputStateInfo()
//{
//	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//
//	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//	vertexInputInfo.vertexBindingDescriptionCount = 1;
//	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
//	vertexInputInfo.pVertexBindingDescriptions = &m_InputBinding;
//	vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
//	return vertexInputInfo;
//}
//
//VkPipelineInputAssemblyStateCreateInfo GP2Shader::createInputAssemblyStateInfo()
//{
//	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	inputAssembly.primitiveRestartEnable = VK_FALSE;
//	return inputAssembly;
//}

//void GP2Shader::createDescriptorSetLayout(const VkDevice& vkDevice)
//{
//	VkDescriptorSetLayoutBinding uboLayoutBinding{};
//	uboLayoutBinding.binding = 0;
//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	uboLayoutBinding.descriptorCount = 1;
//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
//
//	VkDescriptorSetLayoutCreateInfo layoutInfo{};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = 1;
//	layoutInfo.pBindings = &uboLayoutBinding;
//
//	if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor set layout!");
//	}
//}
//
//void GP2Shader::createDescriptorSets(const VkDevice& vkDevice)
//{
//	m_DescriptorPool->CreateDescriptorSets(m_DescriptorSetLayout, { m_UBOBuffer->GetVkBuffer() });
//}
//
//void GP2Shader::bindDescriptorSetLayout(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index)
//{
//	m_DescriptorPool->BindDescriptorSet(commandBuffer, pipelineLayout, index);
//}

void GP2Shader::updateUniformBuffer(uint32_t currentImage, float aspectRatio, float fov)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	VertexUBO ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	m_UBOBuffer->Upload(ubo);
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