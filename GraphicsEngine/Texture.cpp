#include "Texture.h"

Texture::Texture()
{
	m_pTexture = 0;
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_pTexture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	// Release the texture resource.
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = 0;
	}

	return;
}