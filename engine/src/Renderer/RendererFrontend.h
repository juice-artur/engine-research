#pragma once
#include "RendererBackend.h"

class RendererFrontend
{
public:
	bool Initialize(const char* applicationName, CBaseWindow* window);
	bool DrawFrame(float deltaTime);
	void Shutdown();
	void OnResized(unsigned int width, unsigned int height);
private:
	RendererBackend* renderrerBackend;
};
