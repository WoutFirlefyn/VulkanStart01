#pragma once

#include <vector>
#include <memory>
#include "Buffer.h"
#include "UniformBufferObject.h"
template<class UBO>
class DescriptorPool
{
public:
	DescriptorPool(VkDevice device, size_t count);
	~DescriptorPool();

	void Initialize(const VulkanContext& context);
	void SetUBO(UBO data, size_t index);
	const VkDescriptorSetLayout& GetDescriptorSetLayout(){ return m_DescriptorSetLayout; }
	void CreateDescriptorSets();
	void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);
private:
	VkDevice m_Device;
	VkDeviceSize m_Size;
	VkDescriptorSetLayout m_DescriptorSetLayout;

	void CreateDescriptorSetLayout(const VulkanContext& context);
	void CreateUBOs(const VulkanContext& context);

	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_vDescriptorSets;
	std::vector<UniformBufferObjectPtr<UBO>> m_vUBOs;

	size_t m_Count;
};

template<class UBO>
inline DescriptorPool<UBO>::DescriptorPool(VkDevice device, size_t count)
	: m_Device{ device }
	, m_Size{ sizeof(UBO) }
	, m_Count(count)
	, m_DescriptorPool{ nullptr }
	, m_DescriptorSetLayout{ nullptr }
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(count);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = count;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

template <class UBO>
DescriptorPool<UBO>::~DescriptorPool()
{
	for (UniformBufferObjectPtr<UBO>& buffer : m_vUBOs)
	{
		buffer.reset();
	}
	vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
}

template<class UBO>
inline void DescriptorPool<UBO>::Initialize(const VulkanContext& context)
{
	CreateDescriptorSetLayout(context);
	CreateUBOs(context);
	CreateDescriptorSets();
}

template <class UBO>
void DescriptorPool<UBO>::CreateDescriptorSets()
{

	std::vector<VkDescriptorSetLayout> layouts(m_Count, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = m_Count;
	allocInfo.pSetLayouts = layouts.data();

	m_vDescriptorSets.resize(m_Count);
	if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_vDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}


	size_t descriptorIndex = 0;
	for (UniformBufferObjectPtr<UBO>& buffer : m_vUBOs)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffer->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = m_Size;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_vDescriptorSets[descriptorIndex];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
		++descriptorIndex;
	}

}

template <class UBO>
void DescriptorPool<UBO>::BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_vDescriptorSets[index], 0, nullptr);
}

template<class UBO>
inline void DescriptorPool<UBO>::CreateDescriptorSetLayout(const VulkanContext& context)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	if (vkCreateDescriptorSetLayout(context.device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

template<class UBO>
inline void DescriptorPool<UBO>::CreateUBOs(const VulkanContext& context)
{
	for (int uboIndex = 0; uboIndex < m_Count; ++uboIndex)
	{
		UniformBufferObjectPtr<UBO> buffer = std::make_unique<UniformBufferObject<UBO>>();
		buffer->Initialize(context);
		m_vUBOs.emplace_back(std::move(buffer));
	}
}

template<class UBO>
inline void DescriptorPool<UBO>::SetUBO(UBO src, size_t index)
{
	if (index < m_vUBOs.size())
	{
		m_vUBOs[index]->SetData(src);
		m_vUBOs[index]->Upload();
	}
}
