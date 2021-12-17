#pragma once

#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include <fstream>
#include "Structures.h"
#include "Vector3.h"

using namespace DirectX;

class Terrain
{
private:
	ID3D11Buffer* _pGridVertexBuffer;
	ID3D11Buffer* _pGridIndexBuffer;
	XMFLOAT4X4 _gridWorld;
	float _gridWidth, _gridHeight;
	int _heightmapWidth, _heightmapHeight;
	float _heightScale;
	std::string _heightmapFilename;
	std::vector<float> _heightmap;
	int _indexCount;
public:
	Terrain();
	~Terrain();

	HRESULT GenerateGrid(int width, int height, ID3D11Device* pd3dDevice);
	void LoadHeightmap();

	void Update();
	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);
};

