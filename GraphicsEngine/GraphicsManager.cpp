#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_pD3DManager = 0;
	m_pModel = 0;
	m_pCamera = 0;
	m_pColorShader = 0;
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
	m_pCamera = new CameraClass();
	if (!m_pCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_pCamera->SetPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	m_pModel = new ModelClass();
	if (!m_pModel)
	{
		return false;
	}

	// Initialize the model object.
	bInitialize = m_pModel->Initialize(m_pD3DManager->GetDevice());
	if (!bInitialize)
	{
		MessageBox(pWindowHandle, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_pColorShader = new ColorShaderClass;
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

	return true;
}

void GraphicsManager::Release()
{
	// Release the color shader object.
	if (m_pColorShader)
	{
		m_pColorShader->Shutdown();
		delete m_pColorShader;
		m_pColorShader = 0;
	}

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

	// Render the model using the color shader.
	result = m_pColorShader->Render(m_pD3DManager->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	// Present the rendered scene to the screen.
	m_pD3DManager->EndScene();

	return true;
}