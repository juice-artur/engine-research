#pragma once

#include "Platform\Common\Types.h"
#include "vulkan\vulkan.h"
#include <Core\Log.h>



FORCEINLINE const achar** VK_Plat_GetRequiredInstanceExtensions(uint32& size)
{
	static const achar* s_platInstanceExtensions[] =
	{
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	size = (sizeof(s_platInstanceExtensions) / sizeof((s_platInstanceExtensions)[0]));
	return s_platInstanceExtensions;
}

FORCEINLINE VkSurfaceKHR VK_Plat_CreateSurfaceKHR(VkInstance vkInstance, WindowHandle_t windowHandle)
{
	VkSurfaceKHR	vkSurface;
	assert(windowHandle);
	// Create Vulkan surface for Windows
	VkWin32SurfaceCreateInfoKHR				vkWin32SurfaceCreateInfoKHR = {};
	vkWin32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkWin32SurfaceCreateInfoKHR.hwnd = (HWND)windowHandle;
	vkWin32SurfaceCreateInfoKHR.hinstance = GetModuleHandle(NULL);
	vkWin32SurfaceCreateInfoKHR.pNext = NULL;
	vkWin32SurfaceCreateInfoKHR.flags = 0;

	if (vkCreateWin32SurfaceKHR(vkInstance, &vkWin32SurfaceCreateInfoKHR, NULL, &vkSurface) != VK_SUCCESS)
	{
		LOG_WARN("Vulkan: Fail to create Vulkan surface in window handle {0}", windowHandle);
		return VK_NULL_HANDLE;
	}

	return vkSurface;
}