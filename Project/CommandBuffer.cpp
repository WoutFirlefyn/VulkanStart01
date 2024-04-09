//---------------------------
// Includes
//---------------------------
#include "CommandBuffer.h"
#include "CommandPool.h"

//---------------------------
// Member functions
//---------------------------

void CommandBuffer::Reset() const
{
	vkResetCommandBuffer(m_CommandBuffer, 0);
}

void CommandBuffer::BeginRecording() const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	
	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");
}

void CommandBuffer::EndRecording() const
{
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");
}

void CommandBuffer::FreeBuffer(const VkDevice& device, const CommandPool& commandPool) const
{
	vkFreeCommandBuffers(device, commandPool.GetCommandPool(), 1, &m_CommandBuffer);
}

void CommandBuffer::Submit(VkSubmitInfo& info) const
{
	info.commandBufferCount = 1;
	info.pCommandBuffers = &m_CommandBuffer;
}
