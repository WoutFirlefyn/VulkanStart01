#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Buffer.h"
#include "UniformBufferObject.h"
#include "Texture.h"
template<class UBO>
class DescriptorPool
{
public:
	DescriptorPool(VkDevice device, size_t count, bool shouldUseTextures);
	~DescriptorPool();

	template<typename Mesh>
	void Initialize(const VulkanContext& context, std::vector<std::unique_ptr<Mesh>>& vMeshes);
	void SetUBO(UBO data, size_t index);
	const VkDescriptorSetLayout& GetDescriptorSetLayout(){ return m_DescriptorSetLayout; }
	template<typename Mesh>
	void CreateDescriptorSets(std::vector<std::unique_ptr<Mesh>>& vMeshes);
	void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);
private:
	VkDevice m_Device;
	VkDeviceSize m_Size;
	VkDescriptorSetLayout m_DescriptorSetLayout;

	void CreateDescriptorSetLayout(const VulkanContext& context);
	void CreateUBOs(const VulkanContext& context);

	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_vDescriptorSets{};
	std::vector<UniformBufferObjectPtr<UBO>> m_vUBOs;

	size_t m_Count;
	bool m_TexturesEnabled;
};

template<class UBO>
inline DescriptorPool<UBO>::DescriptorPool(VkDevice device, size_t count, bool shouldUseTextures)
	: m_Device{ device }
	, m_Size{ sizeof(UBO) }
	, m_Count(count)
	, m_DescriptorPool{ nullptr }
	, m_DescriptorSetLayout{ nullptr }
	, m_TexturesEnabled{ shouldUseTextures }
{
	std::vector<VkDescriptorPoolSize> poolSizes(1);
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(count);
	if(m_TexturesEnabled)
	{
		poolSizes.resize(2);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(count);
	}

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
template<typename Mesh>
inline void DescriptorPool<UBO>::Initialize(const VulkanContext& context, std::vector<std::unique_ptr<Mesh>>& vMeshes)
{
	CreateDescriptorSetLayout(context);
	CreateUBOs(context);
	CreateDescriptorSets(vMeshes);
}

template <class UBO>
template<typename Mesh>
void DescriptorPool<UBO>::CreateDescriptorSets(std::vector<std::unique_ptr<Mesh>>& vMeshes)
{
	std::vector<VkDescriptorSetLayout> layouts(m_Count, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Count);
	allocInfo.pSetLayouts = layouts.data();

	m_vDescriptorSets.resize(m_Count);
	if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_vDescriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate descriptor sets!");

	for (size_t i = 0; i < vMeshes.size(); ++i) 
	{	
		auto pTexture = vMeshes[i]->GetTexture();

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_vUBOs[i]->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = m_Size;

		std::vector<VkWriteDescriptorSet> descriptorWrites(1);
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_vDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		if (pTexture)
		{
			descriptorWrites.resize(2);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = pTexture->GetTextureImageView();
			imageInfo.sampler = pTexture->GetTextureSampler();

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_vDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;
		}

		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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
	std::vector<VkDescriptorSetLayoutBinding> bindings(1);
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr; // Optional

	if (m_TexturesEnabled)
	{
		bindings.resize(2);
		bindings[1].binding = 1;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].pImmutableSamplers = nullptr;
		bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	if (vkCreateDescriptorSetLayout(context.device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout!");
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
