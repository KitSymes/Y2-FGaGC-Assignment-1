#pragma once

#ifndef _GEOMETRY_H
#define _GEOMETRY_H
#include <d3d11_1.h>
#include "OBJLoader.h"

class Geometry
{
private:
	MeshData* _mesh;
	ID3D11ShaderResourceView* _texture;
public:
	Geometry(MeshData* mesh, ID3D11ShaderResourceView* texture);
	~Geometry();

	void Draw(ID3D11DeviceContext* _pImmediateContext, ID3D11Buffer* _pConstantBuffer);
};
#endif
