#include "Framebuffer.h"

void Framebuffer::Create(std::vector <VkImageView>& attachments, VulkanDevice& device, Renderpass* renderpass, uint32  attachmentCount, glm::ivec2 extend)
{
    this->attachmentCount = attachmentCount;
    this->attachments.resize(attachmentCount);
    for (uint32 i = 0; i < attachmentCount; ++i) {
        this->attachments[i] = attachments[i];
    }
    this->renderpass = renderpass;

    // Creation info
    VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    framebuffer_create_info.renderPass = renderpass->handle;
    framebuffer_create_info.attachmentCount = attachmentCount;
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = extend.x;
    framebuffer_create_info.height = extend.y;
    framebuffer_create_info.layers = 1;

    VK_CHECK(vkCreateFramebuffer(
        device.logicalDevice,
        &framebuffer_create_info,
        /*context->allocator*/nullptr,
        &handle));
}

void Framebuffer::Destroy(VulkanDevice& device)
{
    vkDestroyFramebuffer(device.logicalDevice, handle, /*context->allocator*/nullptr);
    if (!attachments.empty()) {
        attachments.clear();
    }
    handle = nullptr;
    attachmentCount = 0;
    renderpass = nullptr;
}
