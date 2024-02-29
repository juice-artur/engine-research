#pragma once
#include <Platform\Common\Types.h>
#include <vulkan\vulkan.h>
#include <glm\ext\vector_int2.hpp>
#include <Renderer\Vulkan\Devices\VulkanDevice.h>
class Image
{
public:
    Image() {};
    ~Image() {};
    void Create(glm::ivec2 extend, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags, bool createView, VkImageAspectFlags viewAspectFlags, VulkanDevice& device);
    void VulkanImageViewCreate(
        VulkanDevice& device,
        VkFormat format,
        VkImageAspectFlags aspectFlags);
    void Destroy(VulkanDevice& device);

    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;

private:
    glm::ivec2 extend;
};
