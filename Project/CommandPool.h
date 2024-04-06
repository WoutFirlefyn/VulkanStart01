#pragma once

//-----------------------------------------------------
// Include Files
//-----------------------------------------------------
#include "vulkan/vulkan_core.h"
#include "vulkanbase/VulkanUtil.h"
#include "CommandBuffer.h"

//-----------------------------------------------------
// CommandPool Class									
//-----------------------------------------------------
struct QueueFamilyIndices;
class CommandPool final
{
public:													
	CommandPool() : m_CommandPool{ VK_NULL_HANDLE }, m_VkDevice{ VK_NULL_HANDLE }
	{
	}

	~CommandPool() = default;

	// -------------------------
	// Copy/move constructors and assignment operators
	// -------------------------    
	CommandPool(const CommandPool& other)					= delete;
	CommandPool(CommandPool&& other) noexcept				= delete;
	CommandPool& operator=(const CommandPool& other)		= delete;
	CommandPool& operator=(CommandPool&& other)	noexcept	= delete;

	//-------------------------------------------------
	// Member functions						
	//-------------------------------------------------
	void Initialize(const VkDevice& device, const QueueFamilyIndices& queue);
	void Destroy();

	CommandBuffer CreateCommandBuffer()const;

	const VkCommandPool& GetCommandPool() const { return m_CommandPool; }

private:
	VkCommandPool m_CommandPool{};
	VkDevice m_VkDevice{};
};

 
