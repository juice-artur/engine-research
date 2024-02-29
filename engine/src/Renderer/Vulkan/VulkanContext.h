#pragma once

#include "vulkan\vulkan.h"
#include "Renderer\Vulkan\VulkanPlatform.h"
#include "Devices\VulkanDevice.h"
#include <glm\ext\vector_int2.hpp>
#include "Renderpass\Swapchain.h"
#include "Renderpass\Renderpass.h"
#include "Renderpass\Fence.h"

class VulkanContext
{
public:

	VulkanContext();
	~VulkanContext();

	bool Initialize(WindowHandle_t windowHandle);

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_types,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data);
	std::vector<CommandBuffer>  graphicsCommandBuffers;
	Swapchain swapchain;
	VulkanDevice* device;
	Renderpass mainRenderpass;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector <VkSemaphore> queueCompleteSemaphores;

	uint32 inFlightFenceCount;
	std::vector <Fence> inFlightFences;

	// Holds pointers to fences which exist and are owned elsewhere.
	std::vector<Fence*> imagesInFlight;
	VkAllocationCallbacks* allocator;
private:
	VkInstance instance;
#if defined(_DEBUG)
	VkDebugUtilsMessengerEXT debugMessenger;
#endif
	VkSurfaceKHR surface;
	glm::ivec2 framebufferSize;
	uint32 imageIndex;

	bool recreatingSwapchain;
};
