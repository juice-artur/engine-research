#pragma once
#include "vulkan\vulkan.h"
#include <Renderer\Vulkan\VulkanTypes.h>
class CommandBuffer
{
public:
    VkCommandBuffer handle;

    CommandBufferState state;
private:

};
