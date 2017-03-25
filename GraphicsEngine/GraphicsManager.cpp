#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_pD3DManager = 0;
	m_pModel = 0;
	m_pCamera = 0;
#ifdef USE_TEXTURES
	m_pTextureShader = 0;
#else
	m_pColorShader = 0;
#endif
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

	// Create the camera object.
	m_pCamera = new Camera();
	if (!m_pCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_pCamera->SetPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	m_pModel = new Model();
	if (!m_pModel)
	{
		return false;
	}

	// Initialize the model object.

#ifdef USE_TEXTURES
	bInitialize = m_pModel->Initialize(m_pD3DManager->GetDevice(), L"../GraphicsEngine/assets/beachstones.dds");
#else
	bInitialize = m_pModel->Initialize(m_pD3DManager->GetDevice());
#endif
	if (!bInitialize)
	{
		MessageBox(pWindowHandle, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

#ifdef USE_TEXTURES
	// Create the color shader object.
	m_pTextureShader = new TextureShaderClass();
	if (!m_pTextureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	bInitialize = m_pTextureShader->Initialize(m_pD3DManager->GetDevice(), pWindowHandle);
	if (!bInitialize)
	{
		MessageBox(pWindowHandle, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
#else
	// Create the color shader object.
	m_pColorShader = new ColorShader();
	if (!m_pColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	bInitialize = m_pColorShader->Initialize(m_pD3DManager->GetDevice(), pWindowHandle);
	if (!bInitialize)
	{
		MessageBox(pWindowHandle, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
#endif
	return true;
}

void GraphicsManager::Release()
{
#ifdef USE_TEXTURES
	// Release Texture Shader Object.
	if (m_pTextureShader)
	{
		m_pTextureShader->Shutdown();
		delete m_pTextureShader;
		m_pTextureShader = 0;
	}
#else
	// Release the color shader object.
	if (m_pColorShader)
	{
		m_pColorShader->Shutdown();
		delete m_pColorShader;
		m_pColorShader = 0;
	}
#endif

	// Release the model object.
	if (m_pModel)
	{
		m_pModel->Shutdown();
		delete m_pModel;
		m_pModel = 0;
	}

	// Release the camera object.
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = 0;
	}

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
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_pD3DManager->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_pCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_pCamera->GetViewMatrix(viewMatrix);
	m_pD3DManager->GetWorldMatrix(worldMatrix);
	m_pD3DManager->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_pModel->Render(m_pD3DManager->GetDeviceContext());

#ifdef USE_TEXTURES
	// Render the model using the texture shader.
	result = m_pTextureShader->Render(m_pD3DManager->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_pModel->GetTexture());
	if (!result)
	{
		return false;
	}
#else
	// Render the model using the color shader.
	result = m_pColorShader->Render(m_pD3DManager->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
#endif

	// Present the rendered scene to the screen.
	m_pD3DManager->EndScene();

	return true;
}