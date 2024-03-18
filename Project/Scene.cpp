#include "Scene.h"
#include "Mesh.h"
#include "Vertex.h"
#include "CommandPool.h"

void Scene::AddMesh(const Mesh& Mesh)
{
	m_vMeshes.push_back(Mesh);
}

void Scene::AddRectangle(float bottom, float left, float top, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue)
{
	Vertex vertices[4]{ {glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f}} };

	Mesh rect{};
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);
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
