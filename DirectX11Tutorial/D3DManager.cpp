#include "D3DManager.h"

//#pragma optimize("", off)
D3DManager::D3DManager():
m_pSwapChain(NULL),
m_pDevice(NULL),
m_pDeviceContext(NULL),
m_pRenderTargetView(NULL),
m_pDepthStencilBuffer(NULL),
m_pDepthStencilState(NULL),
m_pDepthStencilView(NULL),
m_pRasterState(NULL),
m_bVSyncEnabled(false),
m_videoCardMemory(0)
{
}

D3DManager::D3DManager(const D3DManager& d3dManager)
{

}

D3DManager::~D3DManager()
{
}

bool D3DManager::InitDirectX()
{
	return false;
}

bool D3DManager::Init(HWND pWindowHandle, int screenWidth, int screenHeight, float screenDepth, 
	float screenNear, bool bVSync /*= false*/, bool bFullScreen /*= false*/)
{
	m_bVSyncEnabled = bVSync;

	// Create a DirectX graphics interface factory.
	IDXGIFactory* pFactory = NULL;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for primary video card.
	IDXGIAdapter* pAdapter = NULL;
	result = pFactory->EnumAdapters(0, &pAdapter);
	if (FAILED(result))
	{
		return false;
	}

	// Create the primary output (monitor).
	IDXGIOutput* pAdapterOutput = NULL;
	result = pAdapter->EnumOutputs(0, &pAdapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI display format.
	unsigned int numModes = 0;
	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Create Format List.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[numModes];
	if (!pDisplayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, pDisplayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	unsigned int numerator = 0;
	unsigned int denominator = 0;

	for (unsigned int idx = 0; idx < numModes; idx++)
	{
		if (pDisplayModeList[idx].Width == (unsigned int)screenWidth)
		{
			if (pDisplayModeList[idx].Height == (unsigned int)screenHeight)
			{
				numerator = pDisplayModeList[idx].RefreshRate.Numerator;
				denominator = pDisplayModeList[idx].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = pAdapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	unsigned int videoCardDescLength = 0;
	
	if (wcstombs_s(&videoCardDescLength, m_pVideoCardDescription, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	/*
		Now that we have stored the numerator and denominator for the refresh rate and 
		the video card information we can release the structures and interfaces used to 
		get that information.
	*/
	// Release the display mode list.
	delete[] pDisplayModeList;
	pDisplayModeList = NULL;

	// Release the adapter output.
	pAdapterOutput->Release();
	pAdapterOutput = NULL;

	// Release the adapter.
	pAdapter->Release();
	pAdapter = NULL;

	// Release the factory.
	pFactory->Release();
	pFactory = NULL;

	/*
		Now that we have the refresh rate from the system we can start the DirectX initialization. 
		The first thing we'll do is fill out the description of the swap chain. The swap chain is 
		the front and back buffer to which the graphics will be drawn. Generally you use a single 
		back buffer, do all your drawing to it, and then swap it to the front buffer which then displays
		on the user's screen. That is why it is called a swap chain.
	*/

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*
		The next part of the description of the swap chain is the refresh rate. The refresh rate
		is how many times a second it draws the back buffer to the front buffer. If vsync is set
		to true in our graphicsmanager.h header then this will lock the refresh rate to the system
		settings (for example 60hz). That means it will only draw the screen 60 times a second 
		(or higher if the system refresh rate is more than 60). However if we set vsync to false
		then it will draw the screen as many times a second as it can, however this can cause some
		visual artifacts.
	*/

	// Set the refresh rate of the back buffer.
	if (m_bVSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = pWindowHandle;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (bFullScreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	/*
		After setting up the swap chain description we also need to setup one more variable
		called the feature level. This variable tells DirectX what version we plan to use. 
		Here we set the feature level to 11.0 which is DirectX 11. You can set this to 10 or 9 
		to use a lower level version of DirectX if you plan on supporting multiple versions or 
		running on lower end hardware.
	*/
	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	ID3D11Texture2D* pBackBufferPtr = NULL;
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_pDevice->CreateRenderTargetView(pBackBufferPtr, NULL, &m_pRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	pBackBufferPtr->Release();
	pBackBufferPtr = NULL;


	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Now we need to setup the depth stencil description.This allows us to control what type of depth test Direct3D will do for each pixel.

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// With the description filled out we can now create a depth stencil state.

	// Create the depth stencil state.
	result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// With the created depth stencil state we can now set it so that it takes effect.Notice we use the device context to set it.

	// Set the depth stencil state.
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// The next thing we need to create is the description of the view of the depth stencil buffer.We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.After filling out the description we then call the function CreateDepthStencilView to create it.

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	/*
		With that created we can now call OMSetRenderTargets.This will bind the render target view and 
		the depth stencil buffer to the output render pipeline.This way the graphics that the pipeline 
		renders will get drawn to our back buffer that we previously created.With the graphics written 
		to the back buffer we can then swap it to the front and display our graphics on the user's screen.
	*/
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_pDeviceContext->RSSetState(m_pRasterState);

	/*
		The viewport also needs to be setup so that Direct3D can map clip space coordinates
		to the render target space. Set this to be the entire size of the window.
	*/
	D3D11_VIEWPORT viewport;
	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_pDeviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	
	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_pProjectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_pWorldMatrix);
	
	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_pOrthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;

}

void D3DManager::Release()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}

	if (m_pRasterState)
	{
		m_pRasterState->Release();
		m_pRasterState = NULL;
	}

	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = NULL;
	}

	if (m_pDepthStencilState)
	{
		m_pDepthStencilState->Release();
		m_pDepthStencilState = NULL;
	}

	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = NULL;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = NULL;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->Release();
		m_pDeviceContext = NULL;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	}

}

void D3DManager::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// Clear the depth buffer.
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void D3DManager::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_bVSyncEnabled)
	{
		// Lock to screen refresh rate.
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_pSwapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DManager::GetDevice()
{
	return m_pDevice;
}


ID3D11DeviceContext* D3DManager::GetDeviceContext()
{
	return m_pDeviceContext;
}

void D3DManager::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_pProjectionMatrix;
}


void D3DManager::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_pWorldMatrix;
}


void D3DManager::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_pOrthoMatrix;
}

void D3DManager::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_pVideoCardDescription);
	memory = m_videoCardMemory;
}
