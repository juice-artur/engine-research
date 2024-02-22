#pragma once
#include"Core\Log.h"
#include"vector"
#include "vulkan\vulkan.h"
#include "Platform\Common\Types.h"
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			LOG_CRITICAL("Detected Vulkan error: {0}" , err  ); \
			abort();                                                \
		}                                                           \
	} while (0)


struct VulkanPhysicalDeviceRequirements {
	bool graphics;
	bool present;
	bool compute;
	bool transfer;
	std::vector<const char*> device_extension_names;
	bool samplerAnisotropy;
	bool discreteGpu;
};

struct VulkanPhysicalDeviceQueueFamilyInfo {
	uint32 graphicsFamilyIndex;
	uint32 presentFamilyIndex;
	uint32 computeFamilyIndex;
	uint32 transferFamilyIndex;
};

struct VulkanSwapchainSupportInfo {
	VkSurfaceCapabilitiesKHR capabilities;
	uint32 formatCount;
	std::vector<VkSurfaceFormatKHR> formats;
	uint32 presentModeCount;
	std::vector<VkPresentModeKHR> presentModes;
};

enum class RenderPassState {
	READY,
	RECORDING,
	IN_RENDER_PASS,
	RECORDING_ENDED,
	SUBMITTED,
	NOT_ALLOCATED
};

enum class CommandBufferState {
	COMMAND_BUFFER_STATE_READY,
	COMMAND_BUFFER_STATE_RECORDING,
	COMMAND_BUFFER_STATE_IN_RENDER_PASS,
	COMMAND_BUFFER_STATE_RECORDING_ENDED,
	COMMAND_BUFFER_STATE_SUBMITTED,
	COMMAND_BUFFER_STATE_NOT_ALLOCATED
};