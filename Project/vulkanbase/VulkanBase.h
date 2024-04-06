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
#include "Scene.h"
#include "GraphicsPipeline.h"
#include "Utils.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
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
		//m_GradientShader.initialize(physicalDevice, device);
		createRenderPass();
		
		m_GraphicsPipeline2D.Initialize({ device, physicalDevice, renderPass, swapChainExtent });
		m_GraphicsPipeline3D.Initialize({ device, physicalDevice, renderPass, swapChainExtent });
		//m_GradientShader.createDescriptorSetLayout(device);
		//createGraphicsPipeline();

		// week 02
		m_CommandPool.Initialize(device, findQueueFamilies(physicalDevice));
		createDepthResources();
		createFrameBuffers();
		auto mesh2D = std::make_unique<Mesh2D>();
		const std::vector<Vertex2D> vertices2D = 
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		for (const auto& vertex : vertices2D)
			mesh2D->AddVertex(vertex);
		mesh2D->SetIndices(std::vector<uint32_t>{0, 1, 2, 0, 2, 3});
		mesh2D->Initialize(physicalDevice, device, m_CommandPool, graphicsQueue);
		m_GraphicsPipeline2D.AddMesh(std::move(mesh2D));
		auto mesh3D = std::make_unique<Mesh3D>();
		/*const */std::vector<Vertex3D> vertices3D = 
		{
			//{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			//{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			//{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			//{{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

			//{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			//{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			//{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
			//{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		std::vector<uint32_t> indices{};
		ParseOBJ("resources/vehicle.obj", vertices3D, indices);
		for (const auto& vertex : vertices3D)
			mesh3D->AddVertex(vertex);

		//mesh3D->SetIndices(std::vector<uint32_t> { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 });
		mesh3D->SetIndices(indices);
		mesh3D->Initialize(physicalDevice, device, m_CommandPool, graphicsQueue);
		m_GraphicsPipeline3D.AddMesh(std::move(mesh3D));
		//m_Scene.AddRectangle(0.5f, -0.5f, -0.5f, 0.5f, physicalDevice, device, m_CommandPool, graphicsQueue);
		//m_GradientShader.createDescriptorSets(device);
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

		m_GradientShader.destroy(device);
		m_CommandPool.Destroy();
		for (auto framebuffer : swapChainFramebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);

		m_GraphicsPipeline2D.Cleanup({ device, physicalDevice, renderPass, swapChainExtent });
		m_GraphicsPipeline3D.Cleanup({ device, physicalDevice, renderPass, swapChainExtent });

		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		if (enableValidationLayers) 
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		vkDestroySwapchainKHR(device, swapChain, nullptr);

		// this shit should probably have an abstraction
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);
		//

		m_Scene.Destroy(device);

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

	GP2Shader m_GradientShader{
		"shaders/shader.vert.spv",
		"shaders/shader.frag.spv"
	};

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
	Scene m_Scene{};

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void drawFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;
	GraphicsPipeline2D m_GraphicsPipeline2D{
		"shaders/shader.vert.spv",
		"shaders/shader.frag.spv"
	};
	GraphicsPipeline3D m_GraphicsPipeline3D{
		"shaders/objshader.vert.spv",
		"shaders/objshader.frag.spv"
	};


	void createFrameBuffers();
	void createRenderPass(); 
	void createGraphicsPipeline();

	// this shit should probably be in its own class but idc

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
	////

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

	void beginRenderPass(const CommandBuffer& buffer, VkFramebuffer currentBuffer, VkExtent2D extent);
	void endRenderPass(const CommandBuffer& buffer);

	void createSyncObjects();
	void drawFrame();

	////////////
	// Camera //
	////////////
	void keyEvent(int key, int scancode, int action, int mods);
	void mouseMove(GLFWwindow* window, double xpos, double ypos);
	void mouseEvent(GLFWwindow* window, int button, int action, int mods);
	float m_Radius{};
	double m_Rotation{};
	glm::vec2 m_DragStart{};

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};