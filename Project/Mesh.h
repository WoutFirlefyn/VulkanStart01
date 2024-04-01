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

    // Copy constructor
    Mesh(const Mesh& other) = delete;

    // Move constructor
    Mesh(Mesh&& other) noexcept
        : m_VertexBuffer(std::move(other.m_VertexBuffer)),
        m_IndexBuffer(std::move(other.m_IndexBuffer)),
        m_vVertices(std::move(other.m_vVertices)),
        m_vIndices(std::move(other.m_vIndices)) 
    {
    }

    // Copy assignment operator
    Mesh& operator=(const Mesh& other) = delete;

    // Move assignment operator
    Mesh& operator=(Mesh&& other) noexcept 
    {
        if (this != &other) {
            m_VertexBuffer = std::move(other.m_VertexBuffer);
            m_IndexBuffer = std::move(other.m_IndexBuffer);
            m_vVertices = std::move(other.m_vVertices);
            m_vIndices = std::move(other.m_vIndices);
        }
        return *this;
    }

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue);

	void DestroyMesh(const VkDevice& device);

	void Draw(/*VkPipelineLayout pipelineLayout, */const VkCommandBuffer& cmdBuffer) const;

	void AddVertex(const glm::vec2& pos, const glm::vec3& color);
	void AddVertex(const Vertex& vertex); 
    void AddTriangle(uint16_t i1, uint16_t i2, uint16_t i3, uint16_t offset = 0);

	void SetIndices(const std::vector<uint16_t>& vIndices);

	std::vector<Vertex> GetVertices() const { return m_vVertices; }
	void CopyBuffer(const VkDevice& device, const CommandPool& commandPool, const Buffer& stagingBuffer, const Buffer& dstBuffer, VkDeviceSize size, VkQueue graphicsQueue);
private:
	//uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	void CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue);
	void CreateIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue);

	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;
	std::vector<Vertex> m_vVertices{};
	std::vector<uint16_t> m_vIndices{};

	//VertexConstant m_VertexConstant;
};