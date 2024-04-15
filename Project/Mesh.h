#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "Buffer.h"
#include "CommandPool.h"

class Mesh
{
public:
	~Mesh() = default;

    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) noexcept = delete;
    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) noexcept = delete;

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue);
	void Initialize(const VulkanContext& context, const CommandPool& commandPool);

	void DestroyMesh(const VkDevice& device);

	void Draw(VkPipelineLayout pipelineLayout, const VkCommandBuffer& cmdBuffer) const;

	void SetIndices(const std::vector<uint32_t>& vIndices);

	void SetVertexConstant(const MeshData& vertexConstant) { m_VertexConstant = vertexConstant; }
	const MeshData& GetVertexConstant() const { return m_VertexConstant; }

	void CopyBuffer(const VkDevice& device, const CommandPool& commandPool, const Buffer& stagingBuffer, const Buffer& dstBuffer, VkDeviceSize size, VkQueue graphicsQueue);
protected:
	Mesh() = default;
	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;
	std::vector<uint32_t> m_vIndices{};
	MeshData m_VertexConstant{};

private:
	virtual void CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue) = 0;
	void CreateIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue);
};

class Mesh2D : public Mesh
{
public:
	Mesh2D() : Mesh()
	{
	}
	~Mesh2D() = default;
	void AddVertex(const glm::vec2& pos, const glm::vec3& color);
	void AddVertex(const Vertex2D& vertex);
	std::vector<Vertex2D> GetVertices() const { return m_vVertices; }

	static std::unique_ptr<Mesh2D> CreateRectangle(const VulkanContext& context, const CommandPool& commandPool, int top, int left, int bottom, int right);
	static std::unique_ptr<Mesh2D> CreateOval(const VulkanContext& context, const CommandPool& commandPool, glm::vec2 center, glm::vec2 radius, int numberOfSegments);
private:
	virtual void CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue) override;

	std::vector<Vertex2D> m_vVertices{};
};

class Mesh3D : public Mesh
{
public:
	Mesh3D() : Mesh()
	{
	}
	~Mesh3D() = default;
	void AddVertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color);
	void AddVertex(Vertex3D vertex);
	std::vector<Vertex3D> GetVertices() const { return m_vVertices; }

	static std::unique_ptr<Mesh3D> CreateMesh(const std::string& fileName, const VulkanContext& context, const CommandPool& commandPool, const MeshData& vertexConstant = MeshData{ glm::mat4(1) });
private:
	virtual void CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue) override;

	std::vector<Vertex3D> m_vVertices{};
};