#include "Mesh.h"
#include "CommandBuffer.h"
#include "Utils.h"
#include "numbers"


void Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	CreateVertexBuffer(physicalDevice, device, commandPool, graphicsQueue);
	CreateIndexBuffer(physicalDevice, device, commandPool, graphicsQueue);
}

void Mesh::Initialize(const VulkanContext& context, const CommandPool& commandPool)
{
	Initialize(context.physicalDevice, context.device, commandPool, context.graphicsQueue);
}

void Mesh::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}

void Mesh::Draw(VkPipelineLayout pipelineLayout, const VkCommandBuffer& vkCommandBuffer) const
{
	m_VertexBuffer->BindAsVertexBuffer(vkCommandBuffer);
	m_IndexBuffer->BindAsIndexBuffer(vkCommandBuffer);

	vkCmdPushConstants(
		vkCommandBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT, // Stage flag should match the push constant range in the layout
		0, // Offset within the push constant block
		sizeof(MeshData), // Size of the push constants to update
		&m_VertexConstant // Pointer to the data
	);

	vkCmdDrawIndexed(vkCommandBuffer, static_cast<uint32_t>(m_vIndices.size()), 1, 0, 0, 0);
}

void Mesh::AddTriangle(uint32_t i1, uint32_t i2, uint32_t i3, uint32_t offset)
{
}

void Mesh::SetIndices(const std::vector<uint32_t>& vIndices)
{
	m_vIndices = vIndices;
}

void Mesh::CreateIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(decltype(m_vIndices)::value_type) * m_vIndices.size();

	Buffer stagingBuffer{ physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,bufferSize };

	void* data;
	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vIndices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_IndexBuffer= std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	CopyBuffer(device, commandPool, stagingBuffer, *m_IndexBuffer, bufferSize, graphicsQueue);
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

 //////////////////////////////////////////////


 void Mesh2D::AddVertex(const glm::vec2& pos, const glm::vec3& color)
 {
	 m_vVertices.push_back(Vertex2D{ pos, color });
 }

 void Mesh2D::AddVertex(const Vertex2D& vertex)
 {
	 m_vVertices.push_back(vertex);
 }

 std::unique_ptr<Mesh2D> Mesh2D::CreateRectangle(const VulkanContext& context, const CommandPool& commandPool, int top, int left, int bottom, int right)
 {
	 auto rect = std::make_unique<Mesh2D>();
	 const std::vector<Vertex2D> vertices = 
	 {
		 {glm::vec2{left, top} ,glm::vec3{1.0f,0.0f,0.0f}},
		 {glm::vec2{right, top}	,glm::vec3{0.0f,1.0f,0.0f}},
		 {glm::vec2{right, bottom}	,glm::vec3{1.0f,0.0f,0.0f}},
		 {glm::vec2{left, bottom}	,glm::vec3{0.0f,0.0f,1.0f}}
	 };

	 for (const auto& vertex : vertices)
		 rect->AddVertex(vertex);

	 rect->SetIndices(std::vector<uint32_t>{0, 1, 2, 0, 2, 3});

	 rect->Initialize(context, commandPool);
	 return rect;
 }

 std::unique_ptr<Mesh2D> Mesh2D::CreateOval(const VulkanContext& context, float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments)
 {
	 //assert((radiusX > 0 && radiusY > 0));
	 //constexpr float pi = 3.14159265359f;

	 //float radians = pi * 2 / numberOfSegments;

	 //auto oval = std::make_unique<Mesh2D>();

	 //Vertex2D center{ glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f} };
	 //Vertex2D currEdgeVertex{ {}, glm::vec3{0.0f,1.0f,0.0f} };

	 //oval->AddVertex(center);
	 //for (int i = 1; i <= numberOfSegments; i++)
	 //{
		// currEdgeVertex.pos.x = centerX + radiusX * glm::cos(radians * i);
		// currEdgeVertex.pos.y = centerY + radiusY * glm::sin(radians * i);

		// oval->AddVertex(currEdgeVertex);

		// oval.AddIndex(0);
		// oval.AddIndex(i);
		// if (i == numberOfSegments) oval.AddIndex(1);
		// else oval.AddIndex(i + 1);
	 //}

	 //oval->Initialize(context)
	 //return oval;
	 return nullptr;
 }

 void Mesh2D::CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(decltype(m_vVertices)::value_type) * m_vVertices.size();

	Buffer stagingBuffer{ physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,bufferSize };

	void* data;
	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_VertexBuffer = std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	CopyBuffer(device, commandPool, stagingBuffer, *m_VertexBuffer, bufferSize, graphicsQueue);
}

 //////////////////////////////////////////////

void Mesh3D::AddVertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& color)
{
	m_vVertices.push_back(Vertex3D{ pos, normal, color });
}

void Mesh3D::AddVertex(Vertex3D vertex)
{
	vertex.color = { 1,1,1 };
	m_vVertices.push_back(vertex);
}

void Mesh3D::CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(decltype(m_vVertices)::value_type) * m_vVertices.size();

	Buffer stagingBuffer{ physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,bufferSize };

	void* data;
	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, m_vVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_VertexBuffer = std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	CopyBuffer(device, commandPool, stagingBuffer, *m_VertexBuffer, bufferSize, graphicsQueue);
}