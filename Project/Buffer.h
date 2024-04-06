#pragma once

//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"

//-----------------------------------------------------
// Buffer Class									
//-----------------------------------------------------
class Buffer final
{
public:
	Buffer(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkDeviceSize size
	);
	Buffer(
		VulkanContext context,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkDeviceSize size
	);
	~Buffer()
	{
		DestroyBuffer();
	}

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	Buffer(const Buffer& other)						= delete;    
	Buffer(Buffer&& other) noexcept					= delete;
	Buffer& operator=(const Buffer& other)			= delete;
	Buffer& operator=(Buffer&& other) noexcept		= delete;

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	void Upload(void* data);
	void Map();
	void BindAsVertexBuffer(VkCommandBuffer commandBuffer) const;
	void BindAsIndexBuffer(VkCommandBuffer commandBuffer) const;
	const VkBuffer& GetVkBuffer() const { return m_Buffer; }
	const VkDeviceMemory& GetVkBufferMemory() const { return m_BufferMemory; }
	VkDeviceSize GetSizeInBytes() const { return m_VkDeviceSize; }

private: 
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	void DestroyBuffer();
	VkDevice m_VkDevice{};
	VkDeviceSize m_VkDeviceSize{};
	VkBuffer m_Buffer{};
	VkDeviceMemory m_BufferMemory{};	
	void* m_UniformBufferMapped{};
};

 
