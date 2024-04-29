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

	void Initialize(const VulkanContext& context, VkImageView imageView, VkSampler sampler);
	void SetUBO(UBO data, size_t index);
	const VkDescriptorSetLayout& GetDescriptorSetLayout(){ return m_DescriptorSetLayout; }
	void CreateDescriptorSets(VkImageView imageView, VkSampler sampler);
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
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(count);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(count);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(count);

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
inline void DescriptorPool<UBO>::Initialize(const VulkanContext& context, VkImageView imageView, VkSampler sampler)
{
	CreateDescriptorSetLayout(context);
	CreateUBOs(context);
	CreateDescriptorSets(imageView, sampler);
}

template <class UBO>
void DescriptorPool<UBO>::CreateDescriptorSets(VkImageView imageView, VkSampler sampler)
{

	std::vector<VkDescriptorSetLayout> layouts(m_Count, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Count);
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

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;	
		imageInfo.sampler = sampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_vDescriptorSets[descriptorIndex];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_vDescriptorSets[descriptorIndex];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
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
