#include "Renderpass.h"
#include "Swapchain.h"
#include "Renderer\Vulkan\Devices\VulkanDevice.h"
void Renderpass::Create(float depth, uint32 stencil, glm::vec4 rgba, glm::vec4 xywh, Swapchain& swapchain, VulkanDevice& device)
{
    this->xywh = xywh;
    this->rgba = rgba;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    uint32 attachmentDescriptionCount = 2;
    std::vector<VkAttachmentDescription> attachmentDescriptions(attachmentDescriptionCount);

    // Color attachment
    VkAttachmentDescription colorAttachment;
    colorAttachment.format = swapchain.GetImageFormat().format; // TODO: configurable
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // Do not expect any particular layout before render pass starts.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
    colorAttachment.flags = 0;

    attachmentDescriptions[0] = colorAttachment;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;  // Attachment description array index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    // Depth attachment, if there is one
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = device.depthFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1] = depth_attachment;

    // Depth attachment reference
    VkAttachmentReference depth_attachment_reference;
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // TODO: other attachment types (input, resolve, preserve)

    // Depth stencil data.
    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    // Input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    // Attachments used for multisampling colour attachments
    subpass.pResolveAttachments = 0;

    // Attachments not used in this subpass, but must be preserved for the next.
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    // Render pass dependencies. TODO: make this configurable.
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    // Render pass create.
    VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderPassCreateInfo.attachmentCount = attachmentDescriptionCount;
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    renderPassCreateInfo.pNext = 0;
    renderPassCreateInfo.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        device.logicalDevice,
        &renderPassCreateInfo,
        /*allocator*/nullptr,
        &handle));
}

void Renderpass::Destroy(VulkanDevice& device)
{
    if (handle) {
        vkDestroyRenderPass(device.logicalDevice, handle, /*allocator*/nullptr);
        handle = nullptr;
    }
}

void Renderpass::Begin(VkFramebuffer frameBuffer, CommandBuffer* commandBuffer)
{
    VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    beginInfo.renderPass = handle;
    beginInfo.framebuffer = frameBuffer;
    beginInfo.renderArea.offset.x = xywh.x;
    beginInfo.renderArea.offset.y = xywh.y;
    beginInfo.renderArea.extent.width = xywh.w;
    beginInfo.renderArea.extent.height = xywh.z;

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color.float32[0] = rgba.r;
    clearValues[0].color.float32[1] = rgba.g;
    clearValues[0].color.float32[2] = rgba.b;
    clearValues[0].color.float32[3] = rgba.a;
    clearValues[1].depthStencil.depth = depth;
    clearValues[1].depthStencil.stencil = stencil;

    beginInfo.clearValueCount = 2;
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer->handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    commandBuffer->state = CommandBufferState::COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void Renderpass::End(CommandBuffer* commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer->handle);
    commandBuffer->state = CommandBufferState::COMMAND_BUFFER_STATE_RECORDING;
}
