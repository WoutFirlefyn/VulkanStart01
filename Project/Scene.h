#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <vector>
class CommandPool;
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

	//void AddMesh(const Mesh& mesh);
	void AddRectangle(float bottom, float left, float top, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const CommandPool& commandPool, VkQueue graphicsQueue);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void Destroy(const VkDevice& device);
private:
	std::vector<Mesh> m_vMeshes;
};
