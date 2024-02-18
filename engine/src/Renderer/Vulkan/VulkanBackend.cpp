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
	return true;
}

void VulkanBackend::Shutdown()
{
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
