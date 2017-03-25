#pragma once

#include <d3d11.h>
#include <d3dx11tex.h>


class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();
	
	bool Initialize(ID3D11Device* pDevice, WCHAR* pTextureFileName);
	void Shutdown();
	
	ID3D11ShaderResourceView* GetTexture() { return m_pTexture; };

private:
	ID3D11ShaderResourceView* m_pTexture;
};