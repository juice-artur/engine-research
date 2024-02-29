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
		if (context->graphicsCommandBuffers[i].handle) {
			context->graphicsCommandBuffers[i].Free(
				context->device->graphicsCommandPool,
				context->device);
			context->graphicsCommandBuffers[i].handle = nullptr;
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
	return false;
}

bool VulkanBackend::EndFrame(float deltaTime)
{
	return false;
}

void VulkanBackend::CreateCommandBuffers()
{
	if (context->graphicsCommandBuffers.empty()) {
		context->graphicsCommandBuffers.resize(context->swapchain.GetImageCount());
	}

	for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
		if (context->graphicsCommandBuffers[i].handle) {
			context->graphicsCommandBuffers[i].Free(
				context->device->graphicsCommandPool,
				context->device);
		}
		context->graphicsCommandBuffers[i].Allocate(
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