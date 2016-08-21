#include "SystemClass.h"

//#pragma optimize("", off)
SystemClass::SystemClass()
{
	m_pGraphics = nullptr;
	m_pInput = nullptr;
}


SystemClass::~SystemClass()
{
}

bool SystemClass::Init()
{
	ApplicationHandle = this;
	// Initialize width and height to zero.
	int screenWidth = 0;
	int screenHeight = 0;

	//Initialize Windows
	InitializeWindows(screenWidth, screenHeight);

	// Create and Initialize Input Handler
	m_pInput = new InputHandler();

	if (!m_pInput)
	{
		_Printf_format_string_("Failed to create InputHandler Instance.");
		return false;
	}

	m_pInput->Init();

	// Create and Initialize Graphics Manager.
	m_pGraphics = new GraphicsManager();

	if (!m_pGraphics)
	{
		_Printf_format_string_("Failed to create Graphics Manager Instance");
		return false;
	}

	return m_pGraphics->Init(screenWidth, screenHeight, m_pHwnd);
}

void SystemClass::Run()
{
	MSG windowsMessage;
	
	// Init Message.
	ZeroMemory(&windowsMessage, sizeof(MSG));

	bool appLoopDone = false;

	while (!appLoopDone)
	{
		// Handle Windows Message.
		if (PeekMessage(&windowsMessage, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&windowsMessage);
			DispatchMessage(&windowsMessage);
		}

		// If windows signals to end the application than exit out.
		if (windowsMessage.message == WM_QUIT)
		{
			appLoopDone = true;
		}
		else if (!Frame())
		{
			appLoopDone = true;
		}
	}
}

void SystemClass::Release()
{
	// Release Graphics
	if (m_pGraphics)
	{
		m_pGraphics->Release();

		delete m_pGraphics;
		m_pGraphics = nullptr;
	}

	// Release Input Handler
	if (m_pInput)
	{
		m_pInput->Release();

		delete m_pInput;
		m_pInput = nullptr;
	}

	// Shutdown Window.
	ShutdownWindows();	
}

bool SystemClass::Frame()
{
	// Todo [krajpal]  : For now return false i.e. do not render frame
	//					 when esc key is pressed. Create a menu to exit app.

	if (m_pInput->IsKeyDown(VK_ESCAPE))
	{
		// Exit App.
		return false;
	}

	// Do frame processing
	return m_pGraphics->ProcessFrame();
}

// TODO [krajpal] :  Refactor this fuction when we have a menu styled app window.
LRESULT CALLBACK SystemClass::MessageHandler(HWND pWindowsHandle, UINT windowsMessage, WPARAM windowsParam, LPARAM lparam)
{
	switch (windowsMessage)
	{
	case WM_KEYDOWN:
		if (m_pInput)
			m_pInput->SetKeyDown((unsigned int)windowsParam);
		return 0;

	case WM_KEYUP:
		if (m_pInput)
			m_pInput->SetKeyUp((unsigned int)windowsParam);
		return 0;
	
	default:
		return DefWindowProc(pWindowsHandle, windowsMessage, windowsParam, lparam);
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get Instance of this Application.
	m_pHinstance = GetModuleHandle(nullptr);

	// Application Name.
	m_pApplicationName = L"Graphics Tutorial";

	// Setup windows.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_pHinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_pApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register Class
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_pHwnd = CreateWindowEx(WS_EX_APPWINDOW, m_pApplicationName, m_pApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_pHinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_pHwnd, SW_SHOW);
	SetForegroundWindow(m_pHwnd);
	SetFocus(m_pHwnd);

	// Hide the mouse cursor.
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_pHwnd);
	m_pHwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_pApplicationName, m_pHinstance);
	m_pHinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}