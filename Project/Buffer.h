#pragma once

//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include "vulkanbase/VulkanUtil.h"

//-----------------------------------------------------
// Buffer Class									
//-----------------------------------------------------
class Buffer final
{
public:
	Buffer()	= default;				// Constructor
	~Buffer()	= default;				// Destructor

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	Buffer(const Buffer& other)						= default;
	Buffer(Buffer&& other) noexcept					= default;
	Buffer& operator=(const Buffer& other)			= default;
	Buffer& operator=(Buffer&& other)	noexcept	= default;

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, 
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;

	void BindAsVertexBuffer(VkCommandBuffer commandBuffer) const;
	void BindAsIndexBuffer(VkCommandBuffer commandBuffer) const;
	const VkBuffer& GetVkBuffer() const { return m_Buffer; }
	const VkDeviceMemory& GetVkBufferMemory() const { return m_BufferMemory; }

	void DestroyBuffer(const VkDevice& device);
private: 
	VkBuffer m_Buffer{};
	VkDeviceMemory m_BufferMemory{};	
};

 
