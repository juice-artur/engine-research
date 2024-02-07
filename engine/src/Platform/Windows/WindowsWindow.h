#pragma once

#include"Platform\Common\BaseWindow.h"
#include "SDL.h"
#include <SDL_syswm.h>
#include "Platform\Common\WindowEvent.h"
class WindowsWindow : public BaseWindow
{
public:
	virtual WindowHandle_t GetHandle() const override;
	~WindowsWindow();
	WindowsWindow();

	// Inherited via CBaseWindow
	bool Create(const achar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags = SW_Default) override;
	void Close() override;
	void ShowCursor(bool InIsShowCursor) override;
	void ShowWindow(bool InIsShowWindow) override;
	void Maximize() override;
	void Minimize() override;
	bool PollEvent(WindowEvent& OutWindowEvent) override;
	void SetTitle(const achar* InTitle) override;
	void SetSize(uint32 InWidth, uint32 InHeight) override;
	void SetFullscreen(bool InIsFullscreen) override;
	bool IsOpen() const override;
	bool IsShowingCursor() const override;
	bool IsFullscreen() const override;
	void GetSize(uint32& OutWidth, uint32& OutHeight) const override;
private:
	bool			bIsShowCursor;			/**< Is showed cursor */
	bool			bIsFullscreen;			/**< Is enabled fullscreen mode */
	SDL_Window* sdlWindow;				/**< SDL window */
	SDL_SysWMinfo* sdlWindowInfo;			/**< System info from SDL */
	HANDLE			handle;                 /**< OS handle on window */
};