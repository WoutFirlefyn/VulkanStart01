#pragma once

#include <vector>
#include <memory>
#include "Buffer.h"
class DescriptorPool
{
public:
	DescriptorPool(VkDevice device, VkDeviceSize size, size_t count);
	~DescriptorPool();
	void Initialize();
	void CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
		std::initializer_list<VkBuffer> buffers);
	void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index);
private:
	VkDevice m_Device;
	VkDeviceSize m_Size;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;
	size_t m_Count;
};