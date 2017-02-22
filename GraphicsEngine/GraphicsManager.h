#pragma once
// Includes
#include "D3DManager.h"
#include "ModelClass.h"
#include "CameraClass.h"
#include "ColorShaderClass.h"


// Global Defines
#define FULL_SCREEN 1
#define VSYNC_ENABLED 0
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
	// Memeber functions
	bool RenderFrame();

	// Member Variables
	D3DManager*        m_pD3DManager;
	CameraClass*       m_pCamera;
	ModelClass*        m_pModel;
	ColorShaderClass*  m_pColorShader;
};

