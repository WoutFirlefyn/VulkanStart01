#pragma once

#include <vector>
#include <memory>
#include "Buffer.h"
class DAEDescriptorPool
{
public:
	DAEDescriptorPool(VkDevice device, VkDeviceSize size, size_t count);
	~DAEDescriptorPool();
	void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
		std::initializer_list<VkBuffer> buffers);
	void bindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout
		layout, size_t index);
private:
	VkDevice m_Device;
	VkDeviceSize m_Size;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;
	size_t m_Count;
};