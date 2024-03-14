#include "Scene.h"
#include "Mesh.h"
#include "Vertex.h"

void Scene::AddMesh(const Mesh& Mesh)
{
	m_vMeshes.push_back(Mesh);
}

void Scene::AddRectangle(float bottom, float left, float top, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
	Mesh rect = CreateRectangle(bottom, left, top, right);

	rect.Initialize(physicalDevice, device);
	m_vMeshes.push_back(std::move(rect));
}

void Scene::AddRoundedRectangle(float bottom, float left, float top, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
	constexpr float pi = 3.14159265359f;

	float radians = pi / 2 / numberOfSegmentsPerCorner;

	//Upper Rect
	Vertex vertices[4]{ {glm::vec2{left + radiusX, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right - radiusX, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f}} };

	Mesh rect = CreateRectangle(top + radiusY, left + radiusX, top, right - radiusX);

	Vertex startPoint;
	Vertex endPoint;

	startPoint.color = glm::vec3{ 1.0f, 0.f, 0.f };
	endPoint.color = glm::vec3{ 0.0f, 1.f, 0.f };

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[3].pos.x + radiusX * glm::cos(radians * i + pi);
		startPoint.pos.y = vertices[3].pos.y + radiusY * glm::sin(radians * i + pi);

		endPoint.pos.x = vertices[3].pos.x + radiusX * glm::cos(radians * (i + 1) + pi);
		endPoint.pos.y = vertices[3].pos.y + radiusY * glm::sin(radians * (i + 1) + pi);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[3].pos, vertices[3].color);
	}

	//Left Rect
	vertices[0].pos = glm::vec2{ left, top + radiusY };
	vertices[1].pos = glm::vec2{ left + radiusX, top + radiusY };
	vertices[2].pos = glm::vec2{ left + radiusX, bottom - radiusY };
	vertices[3].pos = glm::vec2{ left, bottom - radiusY };

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[2].pos.x + radiusX * glm::cos(radians * i + pi / 2);
		startPoint.pos.y = vertices[2].pos.y + radiusY * glm::sin(radians * i + pi / 2);

		endPoint.pos.x = vertices[2].pos.x + radiusX * glm::cos(radians * (i + 1) + pi / 2);
		endPoint.pos.y = vertices[2].pos.y + radiusY * glm::sin(radians * (i + 1) + pi / 2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[2].pos, vertices[2].color);
	}
	//Middle Rect
	vertices[0].pos = glm::vec2{ left + radiusX, top + radiusY };
	vertices[1].pos = glm::vec2{ right - radiusX, top + radiusY };
	vertices[2].pos = glm::vec2{ right - radiusX, bottom - radiusY };
	vertices[3].pos = glm::vec2{ left + radiusX, bottom - radiusY };

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	//Right Rect
	vertices[0].pos = glm::vec2{ right - radiusX, top + radiusY };
	vertices[1].pos = glm::vec2{ right, top + radiusY };
	vertices[2].pos = glm::vec2{ right, bottom - radiusY };
	vertices[3].pos = glm::vec2{ right - radiusX, bottom - radiusY };

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[0].pos.x + radiusX * glm::cos(radians * i - pi / 2);
		startPoint.pos.y = vertices[0].pos.y + radiusY * glm::sin(radians * i - pi / 2);

		endPoint.pos.x = vertices[0].pos.x + radiusX * glm::cos(radians * (i + 1) - pi / 2);
		endPoint.pos.y = vertices[0].pos.y + radiusY * glm::sin(radians * (i + 1) - pi / 2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[0].pos, vertices[0].color);
	}

	//Bottom Rect
	vertices[0].pos = glm::vec2{ left + radiusX, bottom - radiusY };
	vertices[1].pos = glm::vec2{ right - radiusX, bottom - radiusY };
	vertices[2].pos = glm::vec2{ right - radiusX, bottom };
	vertices[3].pos = glm::vec2{ left + radiusX, bottom };

	rect.AddVertex(vertices[0]);
	rect.AddVertex(vertices[1]);
	rect.AddVertex(vertices[2]);
	rect.AddVertex(vertices[0]);
	rect.AddVertex(vertices[2]);
	rect.AddVertex(vertices[3]);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[1].pos.x + radiusX * glm::cos(radians * i);
		startPoint.pos.y = vertices[1].pos.y + radiusY * glm::sin(radians * i);

		endPoint.pos.x = vertices[1].pos.x + radiusX * glm::cos(radians * (i + 1));
		endPoint.pos.y = vertices[1].pos.y + radiusY * glm::sin(radians * (i + 1));

		rect.AddVertex(startPoint);
		rect.AddVertex(endPoint);
		rect.AddVertex(vertices[1]);
	}

	rect.Initialize(physicalDevice, device);
	m_vMeshes.push_back(std::move(rect));
}

void Scene::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
	constexpr float pi = 3.14159265359f;

	float radiansIncrement = 2 * pi / numberOfSegments;

	const Vertex center{ glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f} };
	Vertex startPoint{ glm::vec2{}, glm::vec3{0.0f,1.0f,0.0f} };
	Vertex endPoint{ glm::vec2{}, glm::vec3{1.0f,0.0f,0.0f} };

	Mesh oval{};
	for (int i = 0; i < numberOfSegments; i++)
	{
		startPoint.pos.x = centerX + radiusX * glm::cos(radiansIncrement * i);
		startPoint.pos.y = centerY + radiusY * glm::sin(radiansIncrement * i);
		endPoint.pos.x = centerX + radiusX * glm::cos(radiansIncrement * (i + 1));
		endPoint.pos.y = centerY + radiusY * glm::sin(radiansIncrement * (i + 1));

		oval.AddVertex(startPoint);
		oval.AddVertex(endPoint);
		oval.AddVertex(center);

	}

	oval.Initialize(physicalDevice, device);
	m_vMeshes.push_back(std::move(oval));
}

Mesh Scene::CreateRectangle(float bottom, float left, float top, float right)
{
	std::vector<Vertex> vertices
	{
		{ glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f} },
		{ glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f} },
		{ glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f} },
		{ glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f} }
	};

	Mesh rect{};
	rect.AddVertex(vertices[0]);
	rect.AddVertex(vertices[1]);
	rect.AddVertex(vertices[2]);
	rect.AddVertex(vertices[0]);
	rect.AddVertex(vertices[2]);
	rect.AddVertex(vertices[3]);

	return rect;
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
