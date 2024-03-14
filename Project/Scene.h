#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <vector>
class Mesh;
class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) noexcept = delete;

	void AddMesh(const Mesh& mesh);
	void AddRectangle(float bottom, float left, float top, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device);
	void AddRoundedRectangle(float bottom, float left, float top, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device);
	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void Destroy(const VkDevice& device);
private:
	Mesh CreateRectangle(float bottom, float left, float top, float right);

	std::vector<Mesh> m_vMeshes;
};
