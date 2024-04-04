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
	~Buffer()
	{
		DestroyBuffer();
	}

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	Buffer(const Buffer& other)						= delete;    
	Buffer(Buffer&& other) noexcept 
	{
		*this = std::move(other);
	}
	Buffer& operator=(const Buffer& other)			= delete;
	Buffer& operator=(Buffer&& other) noexcept 
	{
		if (this != &other) {
			// Release existing resources
			DestroyBuffer();

			// Transfer ownership
			m_VkDevice = other.m_VkDevice;
			m_VkDeviceSize = other.m_VkDeviceSize;
			m_Buffer = other.m_Buffer;
			m_BufferMemory = other.m_BufferMemory;

			// Reset the source object
			other.m_VkDevice = nullptr;
			other.m_VkDeviceSize = 0;
			other.m_Buffer = VK_NULL_HANDLE;
			other.m_BufferMemory = VK_NULL_HANDLE;
		}
		return *this;
	}

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	void Upload(VertexUBO& ubo);
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

 
