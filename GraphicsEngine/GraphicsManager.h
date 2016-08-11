#pragma once
// Includes
#include <Windows.h>

// Global Defines
#define FULL_SCREEN 1
#define VSYNC_ENABLED 1
#define SCREEN_DEPTH 1000.0f
#define SCREEN_NEAR 0.1f

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	bool Init(int screenWidth, int screenHeight, HWND pWindowHandle);
	void Release();

	bool ProcessFrame();

private:
	bool RenderFrame();
};

