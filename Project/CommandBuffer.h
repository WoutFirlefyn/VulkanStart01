#pragma once
//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include "vulkan/vulkan_core.h"
#include "vulkanbase/VulkanUtil.h"

//-----------------------------------------------------
// CommandBuffer Class									
//-----------------------------------------------------
class CommandPool;
class CommandBuffer final
{
public:
	CommandBuffer() = default;				// Constructor
	~CommandBuffer() = default;				// Destructor

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	CommandBuffer(const CommandBuffer& other)					= default;
	CommandBuffer(CommandBuffer&& other) noexcept				= default;
	CommandBuffer& operator=(const CommandBuffer& other)		= default;
	CommandBuffer& operator=(CommandBuffer&& other)	noexcept	= default;

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void SetVkCommandBuffer(const VkCommandBuffer& buffer) { m_CommandBuffer = buffer; }
	VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
	void Reset() const;
	void BeginRecording() const;
	void EndRecording() const;

	void FreeBuffer(const VkDevice& device, const CommandPool& commandPool);

	void Submit(VkSubmitInfo& info) const;

private: 
	VkCommandBuffer m_CommandBuffer{};
};

 
