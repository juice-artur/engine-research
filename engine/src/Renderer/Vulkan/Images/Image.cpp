#include "Image.h"
#include <Renderer\Vulkan\Utils\VulkanHelper.h>

void Image::Create(glm::ivec2 extend, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, bool createView, VkImageAspectFlags viewAspectFlags, VulkanDevice& device)
{
	this->extend = extend;
    VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = extend.x;
    imageCreateInfo.extent.height = extend.y;
    imageCreateInfo.extent.depth = 1;  // TODO: Support configurable depth.
    imageCreateInfo.mipLevels = 4;     // TODO: Support mip mapping
    imageCreateInfo.arrayLayers = 1;   // TODO: Support number of layers in the image.
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable sample count.
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: Configurable sharing mode.

    VK_CHECK(vkCreateImage(device.logicalDevice, &imageCreateInfo, nullptr/*context->allocator*/, &handle));

    // Query memory requirements.
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device.logicalDevice, handle, &memoryRequirements);

    int32 memoryType = FindMemoryIndex(memoryRequirements.memoryTypeBits, memoryFlags, device);
    if (memoryType == -1) {
        LOG_CRITICAL("Required memory type not found. Image not valid.");
    }

    // Allocate memory
    VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryType;
    VK_CHECK(vkAllocateMemory(device.logicalDevice, &memoryAllocateInfo, nullptr/*context->allocator*/, &memory));

    // Bind the memory
    VK_CHECK(vkBindImageMemory(device.logicalDevice, handle, memory, 0));  // TODO: configurable memory offset.

    // Create view
    if (createView) {
        view = nullptr;
        VulkanImageViewCreate(device, format, viewAspectFlags);
    }
}


void Image::VulkanImageViewCreate(
    VulkanDevice& device,
    VkFormat format,
    VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewCreateInfo.image = handle;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

    // TODO: Make configurable
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device.logicalDevice, &viewCreateInfo, nullptr/*context->allocator*/, &view));
}

void Image::Destroy(VulkanDevice& device)
{
    if (view) {
        vkDestroyImageView(device.logicalDevice, view, nullptr/*context->allocator*/);
        view = nullptr;
    }
    if (memory) {
        vkFreeMemory(device.logicalDevice, memory, nullptr/*context->allocator*/);
        memory = 0;
    }
    if (handle) {
        vkDestroyImage(device.logicalDevice, handle, nullptr/*context->allocator*/);
        handle = 0;
    }
}
