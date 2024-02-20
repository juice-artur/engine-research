#pragma once
#include <vulkan\vulkan.h>
#include <Platform\Common\Types.h>
#include <Renderer\Vulkan\Devices\VulkanDevice.h>

static int32 FindMemoryIndex(uint32 typeFilter, uint32 propertyFlags, VulkanDevice& device) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physicalDevice, &memoryProperties);

    for (uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    LOG_WARN("Unable to find suitable memory type!");
    return -1;
}