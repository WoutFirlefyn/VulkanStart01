//---------------------------
// Includes
//---------------------------
#include "Buffer.h"

//---------------------------
// Member functions
//---------------------------

Buffer::Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size)
	: m_VkDevice{ device }
	, m_VkDeviceSize{ size }
{
	CreateBuffer(device, physicalDevice, size, usage, properties);
}

Buffer::Buffer(VulkanContext context, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size)
	: Buffer(context.physicalDevice, context.device, usage, properties, size)
{
}

void Buffer::CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);

	m_VkDevice = device;
	m_VkDeviceSize = size;
}

void Buffer::Upload(void* data)
{
	memcpy(m_UniformBufferMapped, data, m_VkDeviceSize);
}

void Buffer::Map()
{
	vkMapMemory(m_VkDevice, m_BufferMemory, 0, m_VkDeviceSize, 0, &m_UniformBufferMapped);
}

uint32_t Buffer::FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Buffer::BindAsVertexBuffer(VkCommandBuffer commandBuffer)  const
{
	VkBuffer vertexBuffers[] = { m_Buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void Buffer::BindAsIndexBuffer(VkCommandBuffer commandBuffer) const
{
	vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0,VK_INDEX_TYPE_UINT32);
}

void Buffer::DestroyBuffer()
{
	vkDestroyBuffer(m_VkDevice, m_Buffer, nullptr);
	vkFreeMemory(m_VkDevice, m_BufferMemory, nullptr);
}
