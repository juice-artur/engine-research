#pragma once

#include "Renderer\RendererBackend.h"
#include "VulkanContext.h"

class VulkanBackend : public RendererBackend
{
public:
	VulkanBackend();
	~VulkanBackend();

	virtual bool Initialize(const char* application_name, BaseWindow* window) override;
	virtual void Shutdown() override;
	virtual void OnResized(unsigned int width, unsigned height) override;
	virtual bool BeginFrame(float deltaTime) override;
	virtual bool EndFrame(float deltaTime) override;

private:
	VulkanContext* context;
	void CreateCommandBuffers();
	void RegenerateFramebuffers();
};
