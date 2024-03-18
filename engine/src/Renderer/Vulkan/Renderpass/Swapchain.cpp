#include "Swapchain.h"
#include <algorithm>
#include "vulkan/vulkan_core.h"

void Swapchain::Create(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend)
{
    VkExtent2D swapchainExtent = { (uint32)extend.x, (uint32)extend.y };
    bool found = FALSE;
    for (uint32 i = 0; i < device.GetVulkanSwapchainSupportInfo().formatCount; ++i) {
        VkSurfaceFormatKHR format = device.GetVulkanSwapchainSupportInfo().formats[i];
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            imageFormat = format;
            found = TRUE;
            break;
        }
    }

    if (!found) {
        imageFormat = device.GetVulkanSwapchainSupportInfo().formats[0];
    }


    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32 i = 0; i < device.GetVulkanSwapchainSupportInfo().presentModeCount; ++i) {
        VkPresentModeKHR mode = device.GetVulkanSwapchainSupportInfo().presentModes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            break;
        }
    }

    if (device.GetVulkanSwapchainSupportInfo().capabilities.currentExtent.width != UINT32_MAX) {
        swapchainExtent = device.GetVulkanSwapchainSupportInfo().capabilities.currentExtent;
    }

    VkExtent2D min = device.GetVulkanSwapchainSupportInfo().capabilities.minImageExtent;
    VkExtent2D max = device.GetVulkanSwapchainSupportInfo().capabilities.maxImageExtent;
    swapchainExtent.width = std::clamp(swapchainExtent.width, min.width, max.width);
    swapchainExtent.height = std::clamp(swapchainExtent.height, min.height, max.height);

    imageCount = device.GetVulkanSwapchainSupportInfo().capabilities.minImageCount + 1;
    if (device.GetVulkanSwapchainSupportInfo().capabilities.maxImageCount > 0 && imageCount > device.GetVulkanSwapchainSupportInfo().capabilities.maxImageCount) {
        imageCount = device.GetVulkanSwapchainSupportInfo().capabilities.maxImageCount;
    }
    maxFramesInFlight = imageCount;
    VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = imageFormat.format;
    swapchainCreateInfo.imageColorSpace = imageFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (device.graphicsQueueIndex != device.presentQueueIndex) {
        uint32 queueFamilyIndices[] = {
            (uint32)device.graphicsQueueIndex,
            (uint32)device.presentQueueIndex };
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = 0;
    }

    swapchainCreateInfo.preTransform = device.GetVulkanSwapchainSupportInfo().capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(device.logicalDevice, &swapchainCreateInfo, nullptr /*context->allocator*/, &handle));


    imageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device.logicalDevice, handle, &imageCount, 0));
    if (images.empty()) {
        images.resize(imageCount);
    }
    if (views.empty()) {
        views.resize(imageCount);
    }
    VK_CHECK(vkGetSwapchainImagesKHR(device.logicalDevice, handle, &imageCount, images.data()));

    for (uint32 i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(device.logicalDevice, &viewInfo, nullptr/*context->allocator*/, &views[i]));
    }
    depthAttachment.Create(extend, device.depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true, VK_IMAGE_ASPECT_DEPTH_BIT, device);

    LOG_INFO("Swapchain created successfully.");

}

bool Swapchain::AcquireNextImageIndex(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend, uint64 timeout, VkSemaphore imageAvailableSemaphore, VkFence fence, uint32* outImageIndex)
{
    VkResult result = vkAcquireNextImageKHR(
        device.logicalDevice,
        handle,
        timeout,
        imageAvailableSemaphore,
        fence,
        outImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Trigger swapchain recreation, then boot out of the render loop.
        Recreate(device, surface, extend);
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOG_CRITICAL("Failed to acquire swapchain image!");
        return false;
    }

    return TRUE;
}


void Swapchain::Destroy(VulkanDevice& device) {
    depthAttachment.Destroy(device);
    for (uint32 i = 0; i < imageCount; i++) {
        vkDestroyImageView(device.logicalDevice, views[i], nullptr/*context->allocator*/);
    }

    vkDestroySwapchainKHR(device.logicalDevice, handle, nullptr/*context->allocator*/);
}

void Swapchain::Recreate(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend)
{
    Destroy(device);
    Create(device, surface, extend);
}

uint32 Swapchain::GetImageCount()
{
    return imageCount;
}

VkSurfaceFormatKHR Swapchain::GetImageFormat()
{
    return imageFormat;
}

void Swapchain::Present(VulkanDevice& device, VkSurfaceKHR& surface, glm::ivec2 extend, VkSemaphore renderCompleteSemaphore, uint32 presentImageIndex, VkQueue graphicsQueue,
    VkQueue presentQueue, uint32& curentFrame)
{
    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderCompleteSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &handle;
    presentInfo.pImageIndices = &presentImageIndex;
    presentInfo.pResults = 0;

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
        Recreate(device, surface, extend);
    }
    else if (result != VK_SUCCESS) {
        LOG_CRITICAL("Failed to present swap chain image!");
    }
    curentFrame = (curentFrame + 1) % maxFramesInFlight;
}
