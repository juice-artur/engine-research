#include "RendererFrontend.h"
#include "Renderer\Vulkan\VulkanBackend.h"
#include <Core\Log.h>

bool RendererFrontend::Initialize(const char* applicationName, BaseWindow* window)
{
	renderrerBackend = new VulkanBackend();
	if (!renderrerBackend->Initialize(applicationName, window))
	{
		LOG_CRITICAL("Renderer backend failed to initialize. Shutting down.");
		return false;
	}
	

	return false;
}

bool RendererFrontend::DrawFrame(float deltaTime)
{
	renderrerBackend->BeginFrame(deltaTime);
	renderrerBackend->EndFrame(deltaTime);

	return false;
}

void RendererFrontend::Shutdown()
{
	delete renderrerBackend;
}

void RendererFrontend::OnResized(unsigned int width, unsigned int height)
{
}
