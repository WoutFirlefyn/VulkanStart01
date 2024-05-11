#pragma once
#include <vulkan/vulkan_core.h>
#include <vulkanbase/VulkanUtil.h>
#include <string>
#include <vector>
#include <type_traits>
#include "GP2Shader.h"
#include "CommandBuffer.h"
#include "Mesh.h"
#include "Instance.h"

template <typename Mesh>
class GraphicsPipeline
{
public:
	GraphicsPipeline
	(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile, 
		bool instanced
	);

	void Initialize(const VulkanContext& context);
	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
	void Cleanup(const VulkanContext& context);
	void Record(const CommandBuffer& buffer, VkExtent2D extent, const ViewProjection& ubo);
	void AddMesh(std::unique_ptr<Mesh>&& pMesh);
	void SetUBO(const ViewProjection& ubo, size_t uboIndex);
	void SetVertexConstant(const MeshData& vertexConstant);
private:
	void CreateGraphicsPipeline(const VulkanContext& context);
	VkPushConstantRange CreatePushConstantRange();

	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
	std::vector<VkVertexInputBindingDescription> m_BindingDescriptions;

	std::unique_ptr<DescriptorPool<ViewProjection>> m_UBOPool;
	GP2Shader m_Shader;
	VkRenderPass m_RenderPass{};
	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_GraphicsPipeline{};
	std::vector<std::unique_ptr<Mesh>> m_vMeshes{};
	bool m_Instanced{ false };
};

template<typename Mesh>
inline GraphicsPipeline<Mesh>::GraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile, bool instanced)
	: m_Shader{ vertexShaderFile, fragmentShaderFile }
	, m_BindingDescriptions{}
	, m_AttributeDescriptions{}
	, m_Instanced{ instanced }
{
	using Vertex = std::conditional_t<std::is_same_v<Mesh, Mesh2D>, Vertex2D, Vertex3D>;

	m_BindingDescriptions.emplace_back(Vertex::GetBindingDescription());
	m_AttributeDescriptions = Vertex::GetAttributeDescriptions();

	if (m_Instanced) 
	{
		uint32_t startAttrLoc = static_cast<uint32_t>(m_AttributeDescriptions.size());
		m_BindingDescriptions.emplace_back(InstanceVertex::GetBindingDescription());
		auto instancedAttributeDescriptions = InstanceVertex::GetAttributeDescriptions(startAttrLoc);
		m_AttributeDescriptions.insert(m_AttributeDescriptions.end(), instancedAttributeDescriptions.begin(), instancedAttributeDescriptions.end());
	}
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::Initialize(const VulkanContext& context)
{
	m_RenderPass = context.renderPass;
	m_Shader.initialize(context);
	m_UBOPool = std::make_unique<DescriptorPool<ViewProjection>>(context.device, m_vMeshes.size(), std::is_same_v<Mesh, Mesh3D>);
	m_UBOPool->Initialize<Mesh>(context, m_vMeshes);
	CreateGraphicsPipeline(context);
}

template<typename Mesh>
inline VkPipelineVertexInputStateCreateInfo GraphicsPipeline<Mesh>::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pVertexBindingDescriptions = m_BindingDescriptions.data();
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_BindingDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
	return vertexInputInfo;
}

template<typename Mesh>
inline VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline<Mesh>::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::Cleanup(const VulkanContext& context)
{
	vkDestroyPipeline(context.device, m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(context.device, m_PipelineLayout, nullptr);
	m_UBOPool.reset();

	for (auto& pMesh : m_vMeshes)
		pMesh->DestroyMesh(context.device);
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::Record(const CommandBuffer& buffer, VkExtent2D extent, const ViewProjection& ubo)
{
	vkCmdBindPipeline(buffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(buffer.GetVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(buffer.GetVkCommandBuffer(), 0, 1, &scissor);

	for (size_t i{}; i < m_vMeshes.size(); ++i)
	{
		SetUBO(ubo, i);
		m_UBOPool->BindDescriptorSet(buffer.GetVkCommandBuffer(), m_PipelineLayout, i);
		m_vMeshes[i]->Draw(m_PipelineLayout, buffer.GetVkCommandBuffer());
	}
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::AddMesh(std::unique_ptr<Mesh>&& pMesh)
{
	m_vMeshes.push_back(std::move(pMesh));
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::SetUBO(const ViewProjection& ubo, size_t uboIndex)
{
	m_UBOPool->SetUBO(ubo, uboIndex);
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::SetVertexConstant(const MeshData& vertexConstant)
{
	for (auto& mesh : m_vMeshes)
		mesh->SetVertexConstant({ mesh->GetVertexConstant().model * vertexConstant.model });
}

template<typename Mesh>
inline void GraphicsPipeline<Mesh>::CreateGraphicsPipeline(const VulkanContext& context)
{
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_UBOPool->GetDescriptorSetLayout();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	VkPushConstantRange pushConstantRange = CreatePushConstantRange();
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineInfo.stageCount = (uint32_t)m_Shader.getShaderStages().size();
	pipelineInfo.pStages = m_Shader.getShaderStages().data();

	auto vertexInputStateInfo = CreateVertexInputStateInfo();
	auto inputAssemblyStateInfo = CreateInputAssemblyStateInfo();
	pipelineInfo.pVertexInputState = &vertexInputStateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;

	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	m_Shader.destroyShaderModules(context.device);
}

template<typename Mesh>
inline VkPushConstantRange GraphicsPipeline<Mesh>::CreatePushConstantRange()
{
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	// Stage the push constant is accessible from
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(MeshData); // Size of push constant block
	return pushConstantRange;
}
