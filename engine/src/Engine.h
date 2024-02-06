#include "defines.h"
#include "platform\windows\WindowsWindow.h"
#include "Renderer\RendererFrontend.h"

class Engine
{
public:
	JE_API bool Init();
	JE_API void Tick();
private:
	WindowsWindow window;
	RendererFrontend rendererFrontend;
};