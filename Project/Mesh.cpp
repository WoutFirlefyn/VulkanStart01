#include "Mesh.h"
#include "CommandBuffer.h"


void Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool,VkQueue graphicsQueue)
{
	VkDeviceSize size = sizeof(m_vVertices[0]) * m_vVertices.size();

	m_StagingBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(device, m_StagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)size);
	vkUnmapMemory(device, m_StagingBuffer.GetVkBufferMemory());

	m_VertexBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, commandPool, size, graphicsQueue);
}

void Mesh::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.DestroyBuffer(device);
	m_StagingBuffer.DestroyBuffer(device);
}

void Mesh::Draw(const VkCommandBuffer& cmdBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer.GetVkBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(cmdBuffer, static_cast<uint32_t>(m_vVertices.size()), 1, 0, 0);
}

void Mesh::AddVertex(const glm::vec2& pos, const glm::vec3& color)
{
	m_vVertices.push_back(Vertex{ pos, color });
}

void Mesh::AddVertex(const Vertex& vertex)
{
	AddVertex(vertex.pos, vertex.color);
}

uint32_t Mesh::FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const
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

void Mesh::CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device)
{
	VkDeviceSize bufferSize = sizeof(m_vVertices[0]) * m_vVertices.size();
	m_StagingBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;	
	vkMapMemory(device, m_StagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, m_StagingBuffer.GetVkBufferMemory());
}

 void Mesh::CopyBuffer(const VkDevice& device, const CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue)
 {
	 CommandBuffer commandBuffer = commandPool.CreateCommandBuffer();

	 VkCommandBufferBeginInfo beginInfo{};
	 beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	 beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	 vkBeginCommandBuffer(commandBuffer.GetVkCommandBuffer(), &beginInfo);

	 VkBufferCopy copyRegion{};
	 copyRegion.size = size;
	 vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), m_StagingBuffer.GetVkBuffer(), m_VertexBuffer.GetVkBuffer(), 1, &copyRegion);

	 vkEndCommandBuffer(commandBuffer.GetVkCommandBuffer());

	 VkSubmitInfo submitInfo{};
	 submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	 commandBuffer.Submit(submitInfo);

	 vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	 vkQueueWaitIdle(graphicsQueue);

	 commandBuffer.FreeBuffer(device, commandPool);
 }