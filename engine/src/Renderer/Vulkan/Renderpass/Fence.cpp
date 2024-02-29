#include "Fence.h"

void Fence::Create(VulkanDevice& device, bool createSignaled)
{
	isSignaled = createSignaled;
	VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	if (isSignaled) {
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	}

	VK_CHECK(vkCreateFence(
		device.logicalDevice,
		&fenceCreateInfo,
		nullptr/*context->allocator*/,
		&handle));
}

void Fence::Destroy(VulkanDevice& device)
{
	if (handle) {
		vkDestroyFence(
			device.logicalDevice,
			handle,
			nullptr/*context->allocator*/);
		handle = nullptr;
	}
	isSignaled = false;
}

bool Fence::Wait(VulkanDevice& device, uint64 timeoutNs)
{
	if (!isSignaled) {
		VkResult result = vkWaitForFences(
			device.logicalDevice,
			1,
			&handle,
			true,
			timeoutNs);
		switch (result) {
		case VK_SUCCESS:
			isSignaled = true;
			return true;
		case VK_TIMEOUT:
			LOG_WARN("vk_fence_wait - Timed out");
			break;
		case VK_ERROR_DEVICE_LOST:
			LOG_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST.");
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY.");
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			LOG_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
			break;
		default:
			LOG_ERROR("vk_fence_wait - An unknown error has occurred.");
			break;
		}
	}
	else {
		return true;
	}

	return false;
}

void Fence::Reset(VulkanDevice& device)
{
	if (isSignaled) {
		VK_CHECK(vkResetFences(device.logicalDevice, 1, &handle));
		isSignaled = false;
	}
}
