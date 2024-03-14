#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"

class Mesh
{
public:
	Mesh() = default;
	~Mesh() = default;

	Mesh(const Mesh& other)					= default;
	Mesh(Mesh&& other) noexcept				= default;
	Mesh& operator=(const Mesh& other)		= default;
	Mesh& operator=(Mesh&& other) noexcept	= default;

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device);

	void DestroyMesh(const VkDevice& device);

	void Draw(const VkCommandBuffer& cmdBuffer) const;

	void AddVertex(const glm::vec2& pos, const glm::vec3& color);
	void AddVertex(const Vertex& vertex);

	void CombineMeshes(const Mesh& other);
	std::vector<Vertex> GetVertices() const { return m_vVertices; }

private:
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;

	VkBuffer m_VkBuffer{};
	VkDeviceMemory m_VkBufferMemory{};
	std::vector<Vertex> m_vVertices{};
};