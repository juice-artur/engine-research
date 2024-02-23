#pragma once
#include "vulkan\vulkan.h"
#include <Renderer\Vulkan\VulkanTypes.h>
#include <Renderer\Vulkan\Devices\VulkanDevice.h>
class CommandBuffer
{
public:
	void Allocate(VkCommandPool pool, VulkanDevice* device, bool isPrimary);
	void Free(VkCommandPool pool, VulkanDevice* device);
	void Begin(bool isSingleUse,
		bool isRenderpassContinue,
		bool isSimultaneousUs);
	void End();
	void UpdateSubmitted();
	void Reset();
	void AllocateAndBeginSingleUse(VkCommandPool pool, VulkanDevice* device);
	void EndSingleUse(VkQueue queue, VkCommandPool pool, VulkanDevice* device);
	VkCommandBuffer handle;

	CommandBufferState state;
private:

};
