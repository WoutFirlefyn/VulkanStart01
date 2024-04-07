#pragma once
#include <vulkan/vulkan_core.h>
#include <vulkanbase/VulkanUtil.h>
#include <string>
#include <vector>
#include "GP2Shader.h"
#include "CommandBuffer.h"
#include "Mesh.h"

class GraphicsPipeline2D
{
public:
	GraphicsPipeline2D(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	);
	void Initialize(const VulkanContext& context);
	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
	void Cleanup(const VulkanContext& context);
	void Record(const CommandBuffer& buffer, VkExtent2D extent);
	void DrawScene(const CommandBuffer& buffer);
	void AddMesh(std::unique_ptr<Mesh2D>&& pMesh);
	void SetUBO(ViewProjection ubo, size_t uboIndex);
private:
	void CreateGraphicsPipeline(const VulkanContext& context);
	VkPushConstantRange CreatePushConstantRange();

	std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions;
	VkVertexInputBindingDescription m_InputBinding;

	std::unique_ptr<DescriptorPool<ViewProjection>> m_UBOPool;
	GP2Shader m_Shader;
	VkRenderPass m_RenderPass{};
	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_GraphicsPipeline{};
	std::vector<std::unique_ptr<Mesh2D>> m_vMeshes{};
};

class GraphicsPipeline3D
{
public:
	GraphicsPipeline3D(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	);
	void Initialize(const VulkanContext& context);
	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
	void Cleanup(const VulkanContext& context);
	void Record(const CommandBuffer& buffer, VkExtent2D extent);
	void DrawScene(const CommandBuffer& buffer);
	void AddMesh(std::unique_ptr<Mesh3D>&& pMesh);
	void SetUBO(ViewProjection ubo, size_t uboIndex);
private:
	void CreateGraphicsPipeline(const VulkanContext& context);
	VkPushConstantRange CreatePushConstantRange();

	std::array<VkVertexInputAttributeDescription, 3> m_AttributeDescriptions;
	VkVertexInputBindingDescription m_InputBinding;

	std::unique_ptr<DescriptorPool<ViewProjection>> m_UBOPool;
	GP2Shader m_Shader;
	//CommandBuffer m_CommandBuffer{};
	VkRenderPass m_RenderPass{};
	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_GraphicsPipeline{};
	std::vector<std::unique_ptr<Mesh3D>> m_vMeshes{};
};