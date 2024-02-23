#include "CommandBuffer.h"

void CommandBuffer::Allocate(VkCommandPool pool, VulkanDevice* device, bool isPrimary)
{
    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.commandPool = pool;
    allocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.pNext = 0;

    state = CommandBufferState::COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(
        device->logicalDevice,
        &allocateInfo,
        &handle));
    state = CommandBufferState::COMMAND_BUFFER_STATE_READY;
}

void CommandBuffer::Free(VkCommandPool pool, VulkanDevice* device)
{
    vkFreeCommandBuffers(
        device->logicalDevice,
        pool,
        1,
        &handle);

    handle = nullptr;
    state = CommandBufferState::COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void CommandBuffer::Begin(
    bool isSingleUse,
    bool isRenderpassContinue,
    bool isSimultaneousUse)
{
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = 0;
    if (isSingleUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (isRenderpassContinue) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (isSimultaneousUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_CHECK(vkBeginCommandBuffer(handle, &beginInfo));
    state = CommandBufferState::COMMAND_BUFFER_STATE_RECORDING;
}

void CommandBuffer::End()
{
    VK_CHECK(vkEndCommandBuffer(handle));
    state = CommandBufferState::COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void CommandBuffer::UpdateSubmitted()
{
    state = CommandBufferState::COMMAND_BUFFER_STATE_SUBMITTED;
}

void CommandBuffer::Reset()
{
    state = CommandBufferState::COMMAND_BUFFER_STATE_READY;
}

void CommandBuffer::AllocateAndBeginSingleUse(VkCommandPool pool, VulkanDevice* device)
{
    Allocate(pool, device, true);
    Begin(true, true, false);
}

void CommandBuffer::EndSingleUse(VkQueue queue, VkCommandPool pool, VulkanDevice* device)
{
    End();

    // Submit the queue
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, 0));

    // Wait for it to finish
    VK_CHECK(vkQueueWaitIdle(queue));

    // Free the command buffer.
    Free(pool, device);
}
