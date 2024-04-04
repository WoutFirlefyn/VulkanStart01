#include "Mesh.h"
#include "CommandBuffer.h"


void Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	CreateVertexBuffer(physicalDevice, device, commandPool, graphicsQueue);
	CreateIndexBuffer(physicalDevice, device, commandPool, graphicsQueue);
}

void Mesh::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}

void Mesh::Draw(const VkCommandBuffer& cmdBuffer) const
{
	m_VertexBuffer->BindAsVertexBuffer(cmdBuffer);
	m_IndexBuffer->BindAsIndexBuffer(cmdBuffer);
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_vIndices.size()), 1, 0, 0, 0);
}

void Mesh::AddVertex(const glm::vec2& pos, const glm::vec3& color)
{
	m_vVertices.push_back(Vertex{ pos, color });
}

void Mesh::AddVertex(const Vertex& vertex)
{
	m_vVertices.push_back(vertex);
}

void Mesh::AddTriangle(uint16_t i1, uint16_t i2, uint16_t i3, uint16_t offset)
{
}

void Mesh::SetIndices(const std::vector<uint16_t>& vIndices)
{
	m_vIndices = vIndices;
}

//uint32_t Mesh::FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const
//{
//	VkPhysicalDeviceMemoryProperties memProperties;
//	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
//	{
//		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
//			return i;
//	}
//
//	throw std::runtime_error("failed to find suitable memory type!");
//}

void Mesh::CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(Vertex) * m_vVertices.size();

	Buffer stagingBuffer{  physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,bufferSize };

	void* data;
	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_VertexBuffer = std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	//CopyBuffer(device, commandPool, stagingBuffer, *m_VertexBuffer, bufferSize, graphicsQueue);
}

void Mesh::CreateIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(uint16_t) * m_vIndices.size();

	Buffer stagingBuffer{ physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,bufferSize };

	void* data;
	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vIndices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_IndexBuffer= std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	//CopyBuffer(device, commandPool, stagingBuffer, *m_IndexBuffer, bufferSize, graphicsQueue);
}

 void Mesh::CopyBuffer(const VkDevice& device, const CommandPool& commandPool, const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize size, VkQueue graphicsQueue)
 {
	 CommandBuffer commandBuffer = commandPool.CreateCommandBuffer();

	 VkCommandBufferBeginInfo beginInfo{};
	 beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	 beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	 vkBeginCommandBuffer(commandBuffer.GetVkCommandBuffer(), &beginInfo);

	 VkBufferCopy copyRegion{};
	 copyRegion.size = size;
	 vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), srcBuffer.GetVkBuffer(), dstBuffer.GetVkBuffer(), 1, &copyRegion);

	 vkEndCommandBuffer(commandBuffer.GetVkCommandBuffer());

	 VkSubmitInfo submitInfo{};
	 submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	 commandBuffer.Submit(submitInfo);

	 vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	 vkQueueWaitIdle(graphicsQueue);

	 commandBuffer.FreeBuffer(device, commandPool);
 }