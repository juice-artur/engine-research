#pragma once
#include <Renderer\Vulkan\VulkanTypes.h>
#include "Platform\Common\Types.h"
#include "vulkan\vulkan.h"
#include <glm\ext\vector_float4.hpp>
#include "CommandBuffer.h"
class VulkanDevice;
class Swapchain;


class Renderpass
{
public:
	Renderpass() {};
	~Renderpass() {};
	void Create(float depth, uint32 stencil, glm::vec4 rgba, glm::vec4 xywh, Swapchain& swapchain, VulkanDevice& device);
	void Destroy(VulkanDevice& device);
	void Begin(VkFramebuffer frameBuffer, CommandBuffer* commandBuffer);
	void End(CommandBuffer* commandBuffer);
private:
	float depth;
	uint32 stencil;
	RenderPassState state;
	VkRenderPass handle;
	glm::vec4 rgba;
	glm::vec4 xywh;
};
