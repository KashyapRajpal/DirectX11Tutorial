#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_pD3DManager = NULL;
}


GraphicsManager::~GraphicsManager()
{
	
}

bool GraphicsManager::Init(int screenWidth, int screenHeight, HWND pWindowHandle)
{
	
	// Create the Direct 3D object.
	m_pD3DManager = new D3DManager();

	if (!m_pD3DManager)
	{
		return false;
	}

	// Initialize the Direct3D object.
	bool bInitialize = m_pD3DManager->Init(pWindowHandle, screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, VSYNC_ENABLED, FULL_SCREEN);
	if (!bInitialize)
	{
		MessageBox(pWindowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsManager::Release()
{
	if (m_pD3DManager)
	{
		m_pD3DManager->Release();
		delete m_pD3DManager;
		m_pD3DManager = NULL;
	}
}

bool GraphicsManager::ProcessFrame()
{
	return RenderFrame();
}

bool GraphicsManager::RenderFrame()
{
	// Clear the buffers to begin the scene.
	m_pD3DManager->BeginScene(0.5f, 0.0f, 0.0f, 1.0f);


	// Present the rendered scene to the screen.
	m_pD3DManager->EndScene();

	return true;
}