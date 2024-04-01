#include "Scene.h"
#include "Mesh.h"
#include "Vertex.h"
#include "CommandPool.h"

//void Scene::AddMesh(const Mesh& mesh)
//{
//	m_vMeshes.push_back(std::move(mesh));
//}

void Scene::AddRectangle(float bottom, float left, float top, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	//Vertex vertices[4]{ {glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f}},
	//					{glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f}},
	//					{glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f}},
	//					{glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f}} };
	//Mesh rect{};
	//rect.AddVertex(vertices[0]);
	//rect.AddVertex(vertices[1]);
	//rect.AddVertex(vertices[2]);
	////rect.AddVertex(vertices[0]);
	////rect.AddVertex(vertices[2]);
	//rect.AddVertex(vertices[3]);
	//rect.SetIndices(std::vector<uint16_t>{0, 1, 2, 0, 2, 3});
	//rect.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	//m_vMeshes.push_back(std::move(rect));

	const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	Mesh rect{};

	for (const auto& vertex : vertices)
		rect.AddVertex(vertex);

	rect.SetIndices(std::vector<uint16_t> { 0, 1, 2, 2, 3, 0 });
	rect.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	m_vMeshes.push_back(std::move(rect));
}

void Scene::Draw(const VkCommandBuffer& cmdBuffer) const
{
	for (const auto& mesh : m_vMeshes)
		mesh.Draw(cmdBuffer);
}

void Scene::Destroy(const VkDevice& device)
{
	for (auto& mesh : m_vMeshes)
		mesh.DestroyMesh(device);
}
