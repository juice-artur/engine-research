#pragma once

#include "vulkan\vulkan.h"
#include "Renderer\Vulkan\VulkanPlatform.h"
#include "Devices\VulkanDevice.h"
#include <glm\ext\vector_int2.hpp>
#include "Renderpass\Swapchain.h"

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

private:
	VkInstance instance;
	VkAllocationCallbacks* allocator;
#if defined(_DEBUG)
	VkDebugUtilsMessengerEXT debugMessenger;
#endif
	Swapchain swapchain;
	VkSurfaceKHR surface;
	VulkanDevice *device;

	glm::ivec2 framebufferSize;
	uint32 imageIndex;

	bool recreatingSwapchain;
};
