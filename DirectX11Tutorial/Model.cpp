#include "Model.h"

Model::Model()
{
	m_pVertexBuffer = 0;
	m_pIndexBuffer = 0;
	m_pTexture = 0;
}


Model::Model(const Model& other)
{
}


Model::~Model()
{
}

#ifdef USE_TEXTURES
bool Model::Initialize(ID3D11Device * pDevice, WCHAR * pTextureFileName)
{
	bool result;


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(pDevice);
	if (!result)
	{
		return false;
	}

	return LoadTexture(pDevice, pTextureFileName);
}
#else
bool Model::Initialize(ID3D11Device* pDevice)
{
	bool result;


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(pDevice);
	if(!result)
	{
		return false;
	}

	return true;
}
#endif

void Model::Shutdown()
{
#ifdef USE_TEXTURES
	// Release the model texture.
	ReleaseTexture();
#endif

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}


const int Model::GetIndexCount()
{
	return m_indexCount;
}


bool Model::InitializeBuffers(ID3D11Device* pDevice)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 4;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

#ifdef USE_TEXTURES
	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);;

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Top Left.
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);;

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[2].texture = D3DXVECTOR2(1.0f, 0.0f);;

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);;
#else
	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Top Left.
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[3].color = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
#endif
	// Load the index array with data.

	// Triangle Strip
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 3;  // Bottom Right
	indices[3] = 2;  // Top right.
	
	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}

	return;
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return;
}

#ifdef USE_TEXTURES
bool Model::LoadTexture(ID3D11Device * pDevice, WCHAR * pTextureFileName)
{
	bool result;


	// Create the texture object.
	m_pTexture = new Texture();
	if (!m_pTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_pTexture->Initialize(pDevice, pTextureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	// Release the texture object.
	if (m_pTexture)
	{
		m_pTexture->Shutdown();
		delete m_pTexture;
		m_pTexture = 0;
	}

	return;
}
#endif