#include "VulkanBackend.h"

VulkanBackend::VulkanBackend()
{

}

VulkanBackend::~VulkanBackend()
{
	Shutdown();
}

bool VulkanBackend::Initialize(const char* application_name, BaseWindow* window)
{
	context = new VulkanContext();
	if (!context->Initialize(window->GetHandle()))
	{
		return false;
	}
	context->swapchain.framebuffers.resize(context->swapchain.GetImageCount());
	RegenerateFramebuffers();
	CreateCommandBuffers();


	context->imageAvailableSemaphores.resize(context->swapchain.maxFramesInFlight);
	context->queueCompleteSemaphores.resize(context->swapchain.maxFramesInFlight);
	context->inFlightFences.resize(context->swapchain.maxFramesInFlight);

	for (uint8 i = 0; i < context->swapchain.maxFramesInFlight; ++i) {
		VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore(context->device->logicalDevice, &semaphoreCreateInfo, context->allocator, &context->imageAvailableSemaphores[i]);
		vkCreateSemaphore(context->device->logicalDevice, &semaphoreCreateInfo, context->allocator, &context->queueCompleteSemaphores[i]);

		context->inFlightFences[i].Create(*context->device, true);

	}

	context->imagesInFlight.resize(context->swapchain.GetImageCount());
	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		context->imagesInFlight[i] = nullptr;
	}
	return true;
}

void VulkanBackend::Shutdown()
{
	vkDeviceWaitIdle(context->device->logicalDevice);

	for (uint8 i = 0; i < context->swapchain.maxFramesInFlight; ++i) {
		if (context->imageAvailableSemaphores[i]) {
			vkDestroySemaphore(
				context->device->logicalDevice,
				context->imageAvailableSemaphores[i],
				context->allocator);
			context->imageAvailableSemaphores[i] = nullptr;
		}
		if (context->queueCompleteSemaphores[i]) {
			vkDestroySemaphore(
				context->device->logicalDevice,
				context->queueCompleteSemaphores[i],
				context->allocator);
			context->queueCompleteSemaphores[i] = nullptr;
		}
		context->inFlightFences[i].Destroy(*context->device);
	}
	context->imageAvailableSemaphores.clear();

	context->queueCompleteSemaphores.clear();

	context->inFlightFences.clear();

	context->imagesInFlight.clear();

	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		if (context->graphicsCommandBuffers[i]->handle) {
			context->graphicsCommandBuffers[i]->Free(
				context->device->graphicsCommandPool,
				context->device);
			context->graphicsCommandBuffers[i]->handle = nullptr;
		}
	}
	context->graphicsCommandBuffers.clear();

	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		context->swapchain.framebuffers[i].Destroy(*context->device);
	}

	delete context;
}

void VulkanBackend::OnResized(unsigned int width, unsigned height)
{
}

bool VulkanBackend::BeginFrame(float deltaTime)
{
	if (!context->inFlightFences[context->currentFrame].Wait(*context->device, UINT64_MAX))
	{
		LOG_WARN("In-flight fence wait failure!");
	}

	// Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
	// This same semaphore will later be waited on by the queue submission to ensure this image is available.
	if (!context->swapchain.AcquireNextImageIndex(*context->device, context->surface, { 1280, 720 }, UINT64_MAX, context->imageAvailableSemaphores[context->currentFrame], nullptr, &context->imageIndex)) {
		return FALSE;
	}

	// Begin recording commands.
	CommandBuffer* command_buffer = context->graphicsCommandBuffers[context->imageIndex];
	command_buffer->Reset();
	command_buffer->Begin(false, false, false);

	// Dynamic state
	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 720;
	viewport.width = 1280;
	viewport.height = -720;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Scissor
	VkRect2D scissor;
	scissor.offset.x = scissor.offset.y = 0;
	scissor.extent.width = 1280;
	scissor.extent.height = 720;

	vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);
	vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);

	context->mainRenderpass.xywh.w = 1280;
	context->mainRenderpass.xywh.z = 720;
	context->mainRenderpass.Begin(context->swapchain.framebuffers[context->imageIndex].handle, command_buffer);
}

bool VulkanBackend::EndFrame(float deltaTime)
{
	CommandBuffer* command_buffer = context->graphicsCommandBuffers[context->imageIndex];
	context->mainRenderpass.End(command_buffer);
	command_buffer->End();

	if (context->imagesInFlight[context->imageIndex] != VK_NULL_HANDLE) {
		context->inFlightFences[context->imageIndex].Wait(*context->device, UINT64_MAX);
	}


	context->imagesInFlight[context->imageIndex] = &context->inFlightFences[context->currentFrame];
	context->inFlightFences[context->currentFrame].Reset(*context->device);
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer->handle;

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &context->queueCompleteSemaphores[context->currentFrame];

	// Wait semaphore ensures that the operation cannot begin until the image is available.
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &context->imageAvailableSemaphores[context->currentFrame];

	// Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
	// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
	// writes from executing until the semaphore signals (i.e. one frame is presented at a time)
	VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.pWaitDstStageMask = flags;

	VkResult result = vkQueueSubmit(
		context->device->graphicsQueue,
		1,
		&submit_info,
		context->inFlightFences[context->currentFrame].handle);
	if (result != VK_SUCCESS) {
		LOG_ERROR("vkQueueSubmit failed with result: {0}", result);
		return FALSE;
	}

	command_buffer->UpdateSubmitted();

	context->swapchain.Present(*context->device, context->surface, { 1280, 720 }, context->queueCompleteSemaphores[context->currentFrame], context->imageIndex, context->device->graphicsQueue, context->device->presentQueue, context->currentFrame);
	return false;
}

void VulkanBackend::CreateCommandBuffers()
{
	if (context->graphicsCommandBuffers.empty()) {
		context->graphicsCommandBuffers.resize(context->swapchain.GetImageCount());
		for (size_t i = 0; i < context->swapchain.GetImageCount(); i++)
		{
			context->graphicsCommandBuffers[i] = new CommandBuffer();
		}
	}


	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		if (context->graphicsCommandBuffers[i]->handle) {
			context->graphicsCommandBuffers[i]->Free(
				context->device->graphicsCommandPool,
				context->device);
		}
		context->graphicsCommandBuffers[i]->Allocate(
			context->device->graphicsCommandPool,
			context->device,
			TRUE);
	}

	LOG_TRACE("Vulkan command buffers created.");
}

void VulkanBackend::RegenerateFramebuffers()
{
	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		uint32 attachmentCount = 2;
		std::vector<VkImageView> attachments{
			context->swapchain.views[i],
			context->swapchain.depthAttachment.view };

		context->swapchain.framebuffers[i].Create(attachments, *context->device, &context->mainRenderpass, attachments.size(), { 1280, 720 });
	}
}