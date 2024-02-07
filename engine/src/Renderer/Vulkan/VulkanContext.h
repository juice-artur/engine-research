#pragma once

#include "vulkan\vulkan.h"

class VulkanContext
{
public:
	VulkanContext();
	~VulkanContext();

	bool Initialize();

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
};
