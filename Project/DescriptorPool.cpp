#include "DescriptorPool.h"

DAEDescriptorPool::DAEDescriptorPool(VkDevice device, VkDeviceSize size, size_t count)
{
}

DAEDescriptorPool::~DAEDescriptorPool()
{
}

void DAEDescriptorPool::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, std::initializer_list<VkBuffer> buffers)
{
}

void DAEDescriptorPool::bindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index)
{
}
