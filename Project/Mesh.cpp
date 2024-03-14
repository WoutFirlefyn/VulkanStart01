#include "Mesh.h"


void Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(m_vVertices[0]) * m_vVertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VkBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_VkBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VkBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_VkBuffer, m_VkBufferMemory, 0);

	void* data;
	vkMapMemory(device, m_VkBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, m_VkBufferMemory);
}

void Mesh::DestroyMesh(const VkDevice& device)
{
	vkDestroyBuffer(device, m_VkBuffer, nullptr);
	vkFreeMemory(device, m_VkBufferMemory, nullptr);
}

void Mesh::Draw(const VkCommandBuffer& cmdBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VkBuffer, };
	VkDeviceSize offsets[] = { 0, };
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

void Mesh::CombineMeshes(const Mesh& other)
{
	for (const Vertex& vertex : other.GetVertices())
		AddVertex(vertex);
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