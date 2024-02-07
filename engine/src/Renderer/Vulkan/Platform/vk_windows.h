#pragma once
#include "Platform\Common\Types.h"
#include "vulkan\vulkan.h"


const achar** VK_Plat_GetRequiredInstanceExtensions(uint32& size)
{
	static const achar* s_platInstanceExtensions[] =
	{
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	size = (sizeof(s_platInstanceExtensions) / sizeof((s_platInstanceExtensions)[0]));
	return s_platInstanceExtensions;
}