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
	if(!context->Initialize(window->GetHandle()))
	{
		return false;
	}
    CreateCommandBuffers();
	return true;
}

void VulkanBackend::Shutdown()
{
    for (uint32 i = 0; i < context->swapchain.GetImageCount(); ++i) {
        if (context->graphicsCommandBuffers[i].handle) {
            context->graphicsCommandBuffers[i].Free(
                context->device->graphicsCommandPool,
                context->device);
            context->graphicsCommandBuffers[i].handle = nullptr;
        }
    }
    context->graphicsCommandBuffers.clear();

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
