#include "Engine.h"
#include "Core\Log.h"

JE_API bool Engine::Init()
{
	Log::Init();
	window.Create("ExampleGame", 1280, 720);
	if (!rendererFrontend.Initialize("GameName", &window));
	{
		return false;
	}
	
	return true;
}

JE_API void Engine::Tick()
{
	bool				bExit = false;
	float clock_frequency;
	LARGE_INTEGER frequency;
	LARGE_INTEGER start_time;



	QueryPerformanceFrequency(&frequency);
	clock_frequency = 1.0 / (float)frequency.QuadPart;
	QueryPerformanceCounter(&start_time);
	while(!bExit)
	{
		LARGE_INTEGER now_time;
		QueryPerformanceCounter(&now_time);
		WindowEvent		windowEvent;
		while (window.PollEvent(windowEvent))
		{
			switch (windowEvent.type)
			{
				// Close the window if user pressed on close button
			case WET_WindowClose:
				bExit = true;
				break;
			}
		}
		rendererFrontend.DrawFrame(now_time.QuadPart * clock_frequency);
	}
		rendererFrontend.Shutdown();
}
