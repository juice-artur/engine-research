#pragma once

#include "RendererTypes.h"
#include "Platform\Common\BaseWindow.h"
class RendererBackend
{
public:
	unsigned int frameNumber;


	virtual bool Initialize(const char* application_name, BaseWindow* window) { return false; };
	virtual void Shutdown() {};
	virtual void OnResized(unsigned int width, unsigned height) {};
	virtual bool BeginFrame(float deltaTime) { return false; };
	virtual bool EndFrame(float deltaTime) { return false; };
	virtual ~RendererBackend() {};
};
