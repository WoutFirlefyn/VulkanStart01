#include "Mesh.h"
#include "CommandBuffer.h"
#include "Utils.h"
#include <numbers>


void Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	CreateVertexBuffer(physicalDevice, device, commandPool, graphicsQueue);
	CreateIndexBuffer(physicalDevice, device, commandPool, graphicsQueue);
	glm::mat4 transform;
	const int amountOfMeshesPerSide = static_cast<int>(sqrtf(m_InstanceCount));
	for (int i{}; i < m_InstanceCount; ++i)
	{
		int x = i % amountOfMeshesPerSide;
		int y = i / amountOfMeshesPerSide;
		InstanceVertex instance{};
		transform = glm::translate(glm::mat4(1), (m_InstancedMeshData.maxOffset - m_InstancedMeshData.minOffset) * glm::vec3(x, 0, y) / 2.f);
		m_InstancedMeshData.RandomizeTranslation(transform);
		m_InstancedMeshData.RandomizeRotation(transform);
		m_InstancedMeshData.RandomizeScale(transform);

		instance.modelTransform = GetVertexConstant().model * transform;
		m_vInstanceData.push_back(instance);
	}
	CreateInstancedVertexBuffer(physicalDevice, device, commandPool, graphicsQueue);
}

void Mesh::Initialize(const VulkanContext& context, const CommandPool& commandPool)
{
	Initialize(context.physicalDevice, context.device, commandPool, context.graphicsQueue);
}

void Mesh::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
	m_InstanceBuffer.reset();
	m_pTexture.reset();
}

void Mesh::Draw(VkPipelineLayout pipelineLayout, const VkCommandBuffer& vkCommandBuffer)
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

	if (m_InstanceCount > 1) 
	{
		 m_InstanceBuffer->Upload(m_vInstanceData.data());
		 m_InstanceBuffer->BindAsVertexBuffer(vkCommandBuffer, 1);
	}
	vkCmdDrawIndexed(vkCommandBuffer, static_cast<uint32_t>(m_vIndices.size()), m_InstanceCount, 0, 0, 0);
}

void Mesh::SetIndices(const std::vector<uint32_t>& vIndices)
{
	m_vIndices = vIndices;
}

void Mesh::CreateInstancedVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize bufferSize = sizeof(InstanceVertex) * m_vInstanceData.size();
	m_InstanceBuffer = std::make_unique<Buffer>(physicalDevice, device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize);
	m_InstanceBuffer->Map();
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

 void Mesh::SetInstanceData(uint32_t instanceId, const glm::vec3& t, const glm::vec2& tc)
 {
	if (instanceId < m_InstanceCount)
	{
		m_vInstanceData[instanceId].modelTransform = glm::translate(glm::mat4(1.0f), t);
		m_vInstanceData[instanceId].texCoord = tc;
	}
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
	 return rect;
 }

 std::unique_ptr<Mesh2D> Mesh2D::CreateOval(const VulkanContext& context, const CommandPool& commandPool, glm::vec2 center, glm::vec2 radius, int numberOfSegments)
 {
	 const float pi = static_cast<float>(std::numbers::pi);
	 const float radians = pi * 2 / numberOfSegments;

	 auto oval = std::make_unique<Mesh2D>();

	 Vertex2D centerVertex{ center, glm::vec3{1.0f,0.0f,0.0f} };
	 Vertex2D currEdgeVertex{ {}, glm::vec3{0.0f,0.0f,1.0f} };

	 std::vector<uint32_t> vIndices{};

	 oval->AddVertex(centerVertex);
	 for (int i{}; i < numberOfSegments; i++)
	 {
		 currEdgeVertex.pos = center + radius * glm::vec2(glm::cos(radians * i), glm::sin(radians * i));

		 oval->AddVertex(currEdgeVertex);

		 vIndices.push_back(0);
		 vIndices.push_back(i + 1);
		 if (i == numberOfSegments - 1) 
			 vIndices.push_back(1);
		 else 
			 vIndices.push_back(i + 2);
	 }

	 oval->SetIndices(vIndices);

	 //oval->Initialize(context, commandPool);
	 return oval;
 }

 void Mesh2D::CreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	//m_vInstanceData.resize(m_vVertices.size());
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

std::unique_ptr<Mesh3D> Mesh3D::CreateMesh(const std::string& fileName, std::shared_ptr<Texture> pTexture, const VulkanContext& context, const CommandPool& commandPool)
{
	auto mesh = std::make_unique<Mesh3D>();
	std::vector<Vertex3D> vertices{};
	std::vector<uint32_t> indices{};
	ParseOBJ(fileName, vertices, indices);
	for (const auto& vertex : vertices)
		mesh->AddVertex(vertex);
	
	mesh->SetIndices(indices);
	mesh->SetTexture(pTexture);
	
	return mesh;
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