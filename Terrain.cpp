#include "Terrain.h"

Terrain::Terrain()
{
	_heightScale = 10.0f;
}

Terrain::~Terrain()
{
	if (_pGridVertexBuffer) _pGridVertexBuffer->Release();
	_pGridVertexBuffer = nullptr;
	if (_pGridIndexBuffer) _pGridIndexBuffer->Release();
	_pGridIndexBuffer = nullptr;
}

HRESULT Terrain::GenerateGrid(int width, int height, ID3D11Device* pd3dDevice)
{
	HRESULT hr;

	_gridWidth = width;
	_gridHeight = height;

	_heightmapFilename = "TerrainAssets/minimap.raw";
	LoadHeightmap();

	// Vertex Buffer
	{
		SimpleVertex* finalMesh = new SimpleVertex[(width + 1) * (height + 1)];

		for (int h = 0; h < (height + 1); h++)
			for (int w = 0; w < (width + 1); w++)
			{
				int index = w + (h * (height + 1));
				finalMesh[w + (h * (height + 1))] = { XMFLOAT3(
					w - (_gridWidth / 2.0f),
					_heightmap[index],
					h - (_gridHeight / 2.0f)
				), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2((w / (_gridWidth)), (h / (_gridHeight))) };
			}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * (width + 1) * (height + 1);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = finalMesh;

		hr = pd3dDevice->CreateBuffer(&bd, &InitData, &_pGridVertexBuffer);

		if (FAILED(hr))
			return hr;
	}

	// Index Buffer
	{
		_indexCount = width * height * 2 * 3; // width * height squares, * 2 for each triangle, * 3 for each vertex
		WORD* finalMesh = new WORD[_indexCount];
		for (int i = 0; i < _indexCount; i++)
			finalMesh[i] = 0;
		int i = 0;
		for (int h = 0; h < height; h++)
			for (int w = 0; w < width; w++)
			{
				finalMesh[i] = w + (h * (height + 1)); // Current Vertex
				i++;
				finalMesh[i] = w + ((h + 1) * (height + 1)); // Vertex above
				i++;
				finalMesh[i] = w + 1 + (h * (height + 1)); // Vertex to the right
				i++;

				finalMesh[i] = w + ((h + 1) * (height + 1)); // Vertex above
				i++;
				finalMesh[i] = w + 1 + ((h + 1) * (height + 1)); // Vertex diagonal
				i++;
				finalMesh[i] = w + 1 + (h * (height + 1)); // Vertex to the right
				i++;
			}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * _indexCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = finalMesh;
		hr = pd3dDevice->CreateBuffer(&bd, &InitData, &_pGridIndexBuffer);

		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

void Terrain::LoadHeightmap()
{
	_heightmapWidth = _gridWidth + 1;
	_heightmapHeight = _gridHeight + 1;

	// A height for each vertex
	std::vector<unsigned char> in(_heightmapWidth * _heightmapHeight);

	// Open the file.
	std::ifstream inFile;
	inFile.open(_heightmapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());
		// Done with file.

		inFile.close();
	}

	// Copy the array data into a float array and scale it.
	_heightmap.resize(_heightmapHeight * _heightmapWidth, 0);
	for (UINT i = 0; i < _heightmapHeight * _heightmapWidth; ++i)
		_heightmap[i] = (in[i] / 255.0f) * _heightScale;
}

void Terrain::Update()
{
	XMStoreFloat4x4(&_gridWorld, XMMatrixTranslation(0.0f, -2.0f, 0.0f));
}

void Terrain::Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	// Set to Grid
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pGridVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pGridIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Draw Grid
	XMMATRIX world = XMLoadFloat4x4(&_gridWorld);
	//for (int w = 0; w < _gridWidth; w++)
		//for (int h = 0; h < _gridHeight; h++)
	{
		//cb->mWorld = XMMatrixTranspose(world * XMMatrixTranslation((w - (_gridWidth / 2)) * 2, 0.0f, (h - (_gridHeight / 2)) * 2));
		cb->mWorld = XMMatrixTranspose(world);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, cb, 0, 0);
		_pImmediateContext->DrawIndexed(_indexCount, 0, 0);
		//_pImmediateContext->DrawIndexed(3, 3, 0);
	}
}
