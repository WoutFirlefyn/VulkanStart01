#pragma once
#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>
#include <vector>
struct InstanceVertex 
{
	glm::mat4 modelTransform;
	glm::vec2 texCoord;
	static VkVertexInputBindingDescription GetBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(InstanceVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescription;
	}
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(uint32_t location) 
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(5);
		uint32_t binding = 1;
		for (int i = 0; i < 4; i++) 
		{
			attributeDescriptions[i].binding = binding;
			attributeDescriptions[i].location = location + i;
			attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[i].offset = sizeof(float) * 4 * i;
		}
		attributeDescriptions[4].binding = binding;
		attributeDescriptions[4].location = location + 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(InstanceVertex, texCoord);
		return attributeDescriptions;
	}
};