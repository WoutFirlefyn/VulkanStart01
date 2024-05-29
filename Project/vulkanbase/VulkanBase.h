#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>

#include "GP2Shader.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Vertex.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"
#include "Utils.h"
#include "Camera.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() 
	{
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();
		
		// week 03
		createRenderPass();

		// week 02
		m_CommandPool.Initialize(device, findQueueFamilies(physicalDevice));
		createDepthResources();
		createFrameBuffers();
		
		m_Camera.Initialize(60.f, glm::vec3(0, 2, -15), static_cast<float>(swapChainExtent.width) / swapChainExtent.height);

		VulkanContext context{ device, physicalDevice, renderPass, swapChainExtent, graphicsQueue };

		m_GraphicsPipeline2D.AddMesh(std::move(Mesh2D::CreateRectangle(context, m_CommandPool, 50, 50, 100, 100)));
		m_GraphicsPipeline2D.AddMesh(std::move(Mesh2D::CreateOval(context, m_CommandPool, {75, 150}, {25, 37.5f}, 64)));

		//glm::vec3 offset{ 12, 0, 0 };

		auto pBirbTexture = std::make_shared<Texture>("birb.png", context, m_CommandPool);
		auto pVehicleTexture = std::make_shared<Texture>("vehicle_diffuse.png", context, m_CommandPool);
		auto pGrassTexture = std::make_shared<Texture>("GrassBlock.png", context, m_CommandPool);

		m_GraphicsPipeline3D.AddMesh(std::move(
			Mesh3D::CreateMesh(
				"resources/vehicle.obj",
				pVehicleTexture,
				context, 
				m_CommandPool, 
				MeshData{ glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3{-20, 0 ,0}), glm::vec3(0.5f))},
				1
			)));
		
		m_GraphicsPipelineInstancing.AddMesh(std::move(
			Mesh3D::CreateMesh(
				"resources/birb.obj",
				pBirbTexture,
				context, 
				m_CommandPool, 
				MeshData{ glm::scale(glm::mat4(1), glm::vec3(0.25f)) },
				100000
			)));
		
		m_GraphicsPipelineInstancing.AddMesh(std::move(
			Mesh3D::CreateMesh(
				"resources/cube.obj",
				pGrassTexture,
				context,
				m_CommandPool,
				MeshData{ glm::translate((glm::rotate(glm::mat4(1), glm::radians(90.f), {0,1,0})),{20,0,0}) },
				100000
			)));

		m_GraphicsPipeline2D.Initialize(context);
		m_GraphicsPipeline3D.Initialize(context);
		m_GraphicsPipelineInstancing.Initialize(context);

		m_CommandBuffer = m_CommandPool.CreateCommandBuffer();
		
		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_CommandPool.Destroy();
		for (auto framebuffer : swapChainFramebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);

		m_GraphicsPipeline2D.Cleanup({ device, physicalDevice, renderPass, swapChainExtent });
		m_GraphicsPipeline3D.Cleanup({ device, physicalDevice, renderPass, swapChainExtent });
		m_GraphicsPipelineInstancing.Cleanup({ device, physicalDevice, renderPass, swapChainExtent });

		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		if (enableValidationLayers) 
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		vkDestroySwapchainKHR(device, swapChain, nullptr);

		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();

	// Week 02
	// Queue families
	// CommandBuffer concept

	CommandPool m_CommandPool;
	CommandBuffer m_CommandBuffer;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;
	GraphicsPipeline<Mesh2D> m_GraphicsPipeline2D{
		"shaders/shader.vert.spv",
		"shaders/shader.frag.spv",
		false
	};
	GraphicsPipeline<Mesh3D> m_GraphicsPipeline3D{
		"shaders/objshader.vert.spv",
		"shaders/objshader.frag.spv",
		false
	};
	GraphicsPipeline<Mesh3D> m_GraphicsPipelineInstancing{
		"shaders/instancedobjshader.vert.spv",
		"shaders/instancedobjshader.frag.spv",
		true
	};


	void createFrameBuffers();
	void createRenderPass();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	void createDepthResources(); 
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);
	uint32_t findMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void beginRenderPass(const CommandBuffer& buffer, VkFramebuffer currentBuffer, VkExtent2D extent) const;
	void endRenderPass(const CommandBuffer& buffer);

	void createSyncObjects();
	void drawFrame();

	Camera m_Camera;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};