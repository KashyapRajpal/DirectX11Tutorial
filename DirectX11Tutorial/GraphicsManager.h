#pragma once
// Includes
#include "Defines.h"
#include "D3DManager.h"
#include "Model.h"
#include "Camera.h"

#ifdef USE_TEXTURES
#include "TextureShaderClass.h"
#else
#include "ColorShader.h"
#endif

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
	Camera*			   m_pCamera;
	Model*			   m_pModel;
	
#ifdef USE_TEXTURES
	TextureShaderClass* m_pTextureShader;
#else
	ColorShader*  m_pColorShader;
#endif
};

