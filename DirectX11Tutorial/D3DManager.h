#pragma once

// Linking libraries.
/*
	First thing in the header is to specify the libraries to 
	link when using this object module. These libraries contain
	all the Direct3D functionality for setting up and drawing 
	3D graphics in DirectX as well as tools to interface with 
	the hardware on the computer to obtain information about 
	the refresh rate of the monitor, the video card being used,
	and so forth. You will notice that some DirectX 10 libraries 
	are still used, this is because those libraries were never
	upgraded for DirectX 11 as their functionality did not need
	to change.
*/
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

// Includes
#include <DXGI.h>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX10math.h>

class D3DManager
{
public:
	D3DManager();
	D3DManager(const D3DManager&);
	~D3DManager();

	bool Init(HWND pWindowHandle, int deviceWidth, int deviceHeight, float screenDepth, float screenNear, bool bVSync = false, bool bFullScreen = false);
	void Release();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device*		 GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:

	// Function.
	bool InitDirectX();

	// Member Variables
	bool    m_bVSyncEnabled;
	int     m_videoCardMemory;
	char    m_pVideoCardDescription[128];

	// DX variables.
	IDXGISwapChain*           m_pSwapChain;
	ID3D11Device*             m_pDevice;
	ID3D11DeviceContext*      m_pDeviceContext;
	ID3D11RenderTargetView*   m_pRenderTargetView;
	ID3D11Texture2D*          m_pDepthStencilBuffer;
	ID3D11DepthStencilView*   m_pDepthStencilView;
	ID3D11DepthStencilState*  m_pDepthStencilState;
	ID3D11RasterizerState*    m_pRasterState;

	// MVP Matrix
	D3DXMATRIX	m_pProjectionMatrix, m_pWorldMatrix, m_pOrthoMatrix;
};

