#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "Buffer.h"
#include "CommandPool.h"

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	Mesh(const Mesh& other)					= default;
	Mesh(Mesh&& other) noexcept				= default;
	Mesh& operator=(const Mesh& other)		= default;
	Mesh& operator=(Mesh&& other) noexcept	= default;

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue);

	void DestroyMesh(const VkDevice& device);

	void Draw(const VkCommandBuffer& cmdBuffer) const;

	void AddVertex(const glm::vec2& pos, const glm::vec3& color);
	void AddVertex(const Vertex& vertex);

	std::vector<Vertex> GetVertices() const { return m_vVertices; }
	void CopyBuffer(const VkDevice& device, const CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue);
private:
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	void CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device);

	Buffer m_StagingBuffer{};
	Buffer m_VertexBuffer{};
	std::vector<Vertex> m_vVertices{};
};