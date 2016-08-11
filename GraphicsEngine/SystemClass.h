#pragma once

// To speed up windows build process so that we do not build unnecessary files.
#define WIN32_LEAN_AND_MEAN

#include "InputHandler.h"
#include "GraphicsManager.h"

class SystemClass
{
public:
	SystemClass();
	~SystemClass();

	bool Init();
	void Run();
	void Release();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_pApplicationName;
	HINSTANCE m_pHinstance;
	HWND m_pHwnd;

	InputHandler* m_pInput;
	GraphicsManager* m_pGraphics;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;
