#pragma once
#include <vulkan/vulkan_core.h>
#include "vulkanbase/VulkanUtil.h"
#include "CommandPool.h"

class Texture final
{
public:
	Texture() = default;
	~Texture();

	void Initialize(const std::string& fileName, const VulkanContext& context, CommandPool& commandPool);

	VkImage GetTextureImage() const { return m_TextureImage; }
	VkImageView GetTextureImageView() const { return m_TextureImageView; }
	VkSampler GetTextureSampler() const { return m_TextureSampler; }
private:
	void CreateTextureImage(const std::string& fileName);
	void CreateTextureImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void CreateTextureSampler();

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	uint32_t FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;

	VkImage m_TextureImage{};
	VkDeviceMemory m_TextureImageMemory{};
	VkImageView m_TextureImageView{};
	VkSampler m_TextureSampler{};
	CommandPool* m_CommandPool{};
	VulkanContext m_Context{};
};			