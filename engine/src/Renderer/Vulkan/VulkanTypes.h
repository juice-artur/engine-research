#include"Core\Log.h"

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