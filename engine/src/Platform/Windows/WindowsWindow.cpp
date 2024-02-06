#include "WindowsWindow.h"
#include <Core\Log.h>

WindowHandle_t WindowsWindow::GetHandle() const
{
	return handle;
}

WindowsWindow::~WindowsWindow()
{
	Close();
}

WindowsWindow::WindowsWindow() : bIsShowCursor(false)
, bIsFullscreen(false)
, sdlWindow(nullptr)
, sdlWindowInfo(nullptr)
, handle(nullptr)
{
}

bool WindowsWindow::Create(const achar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags)
{
	if (sdlWindow)
	{
		LOG_WARN("Warning: CWindowsWindow::Create: Window already created");
		return true;
	}

	// Combine flags for SDL2
	uint32		sdlFlags = 0;
	if (InFlags & SW_Hidden)
	{
		sdlFlags = SDL_WINDOW_HIDDEN;
	}
	else
	{
		sdlFlags = SDL_WINDOW_SHOWN;
	}

	if (bIsFullscreen || InFlags & SW_Fullscreen)
	{
		bIsFullscreen = true;
		sdlFlags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		if (InFlags & SW_Resizable)
		{
			sdlFlags |= SDL_WINDOW_RESIZABLE;
		}

		if (!(InFlags & SW_Decorated))
		{
			sdlFlags |= SDL_WINDOW_BORDERLESS;
		}

		if (InFlags & SW_Minimized)
		{
			sdlFlags |= SDL_WINDOW_MINIMIZED;
		}

		if (InFlags & SW_Maximized)
		{
			sdlFlags |= SDL_WINDOW_MAXIMIZED;
		}
	}

	// Create the window
	sdlWindow = SDL_CreateWindow(InTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, InWidth, InHeight, sdlFlags);
	if (!sdlWindow)
	{
		LOG_ERROR("Error: Failed created window ({0}x{1}) with title '{2}' and flags {3}. SDL error: {4}", InWidth, InHeight, InTitle, InFlags, SDL_GetError());
		return false;
	}

	// Get OS handle on window
	sdlWindowInfo = new SDL_SysWMinfo();
	SDL_VERSION(&sdlWindowInfo->version);
	SDL_GetWindowWMInfo(sdlWindow, sdlWindowInfo);
	handle = sdlWindowInfo->info.win.window;
	LOG_INFO("Window with handle {0} successfully created", handle);

	return true;
}

void WindowsWindow::Close()
{
	if (!sdlWindow)
	{
		return;
	}

	SDL_DestroyWindow(sdlWindow);
	delete sdlWindowInfo;

	// Show mouse if it need
	if (!bIsShowCursor)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	sdlWindow = nullptr;
	sdlWindowInfo = nullptr;
	handle = nullptr;
	bIsFullscreen = false;
	bIsShowCursor = false;
	LOG_INFO("Window with handle {0} closed", handle);
}

void WindowsWindow::ShowCursor(bool InIsShowCursor)
{
	bIsShowCursor = InIsShowCursor;
	SDL_SetRelativeMouseMode(InIsShowCursor ? SDL_FALSE : SDL_TRUE);
}

void WindowsWindow::ShowWindow(bool InIsShowWindow)
{
	if (sdlWindow)
	{
		if (InIsShowWindow)
		{
			SDL_ShowWindow(sdlWindow);
		}
		else
		{
			SDL_HideWindow(sdlWindow);
		}
	}
}

void WindowsWindow::Maximize()
{
	if (sdlWindow)
	{
		SDL_MaximizeWindow(sdlWindow);
	}
}

void WindowsWindow::Minimize()
{
	if (sdlWindow)
	{
		SDL_MinimizeWindow(sdlWindow);
	}
}

bool WindowsWindow::PollEvent(WindowEvent& OutWindowEvent)
{
	if (!sdlWindow)
	{
		return false;
	}

	// By default set window event to none
	OutWindowEvent.type = WET_None;

	// Polls for currently pending SDL2 events
	SDL_Event		sdlEvent;
	bool			bIsNotEndQueue = SDL_PollEvent(&sdlEvent);

	// Handle SDL2 events
	switch (sdlEvent.type)
	{
		// Window events
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event)
		{
			// Close window
		case SDL_WINDOWEVENT_CLOSE:
			OutWindowEvent.type = WET_WindowClose;
			break;
		}
		break;

		// Unknown event
	default:
		OutWindowEvent.type = WET_None;
		break;
	}

	return bIsNotEndQueue;
}

void WindowsWindow::SetTitle(const achar* InTitle)
{
	if (sdlWindow)
	{
		SDL_SetWindowTitle(sdlWindow, InTitle);
	}
}

void WindowsWindow::SetSize(uint32 InWidth, uint32 InHeight)
{
	if (sdlWindow)
	{
		SDL_SetWindowSize(sdlWindow, InWidth, InHeight);
		SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

void WindowsWindow::SetFullscreen(bool InIsFullscreen)
{
	if (sdlWindow)
	{
		bIsFullscreen = InIsFullscreen;
		SDL_SetWindowFullscreen(sdlWindow, InIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	}
}

bool WindowsWindow::IsOpen() const
{
	return sdlWindow;
}

bool WindowsWindow::IsShowingCursor() const
{
	return bIsShowCursor;
}

bool WindowsWindow::IsFullscreen() const
{
	return bIsFullscreen;
}

void WindowsWindow::GetSize(uint32& OutWidth, uint32& OutHeight) const
{
	if (sdlWindow)
	{
		SDL_GetWindowSize(sdlWindow, (int*)&OutWidth, (int*)&OutHeight);
	}
}
