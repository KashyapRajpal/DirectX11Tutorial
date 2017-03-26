#pragma once


//////////////
// INCLUDES //
//////////////

// DX11 includes
#include <d3d11.h>
#include <d3dx10math.h>

// File Includes
#include "Defines.h"

#ifdef USE_TEXTURES
#include "Texture.h"
#endif
class Model
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
#ifdef USE_TEXTURES
		D3DXVECTOR2 texture;
#elif
	    D3DXVECTOR4 color;
#endif
	};

public:
	Model();
	Model(const Model&);
	~Model();

#ifdef USE_TEXTURES
	bool Initialize(ID3D11Device* pDevice, WCHAR* pTextureFileName);
#else
	bool Initialize(ID3D11Device*);
#endif
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	const int GetIndexCount();

#ifdef USE_TEXTURES
	ID3D11ShaderResourceView* GetTexture() { return m_pTexture ? m_pTexture->GetTexture() : NULL;	}
#endif

private:
	bool InitializeBuffers(ID3D11Device* pBuffers);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

#ifdef USE_TEXTURES
	bool LoadTexture(ID3D11Device* pDevice, WCHAR* pTextureFileName);
	void ReleaseTexture();
#endif

private:
	ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
	int m_vertexCount, m_indexCount;

#ifdef USE_TEXTURES
	Texture* m_pTexture;
#endif
};