#pragma once

#ifndef _TERRAIN_H
#define _TERRAIN_H
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include <fstream>
#include "Structures.h"
#include "GameObject.h"

using namespace DirectX;

class Terrain :
	public GameObject
{
private:
	ID3D11Buffer* _pGridVertexBuffer;
	ID3D11Buffer* _pGridIndexBuffer;
	float _gridWidth, _gridHeight;
	int _heightmapWidth, _heightmapHeight;
	float _heightScale;
	std::string _heightmapFilename;
	std::vector<float> _heightmap;
	int _indexCount;
public:
	Terrain(Vector3 position);
	~Terrain();

	HRESULT GenerateGrid(int width, int height, ID3D11Device* pd3dDevice);
	void LoadHeightmap();

	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);
};
#endif
