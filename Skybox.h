#pragma once
#ifndef _SKYBOX_H
#define _SKYBOX_H
#include <d3d11.h>
#include "OBJLoader.h"
#include "Vector3.h"
class Skybox
{
private:
	MeshData* _mesh;

	ID3D11ShaderResourceView* _skyboxPX = nullptr;
	ID3D11ShaderResourceView* _skyboxNX = nullptr;
	ID3D11ShaderResourceView* _skyboxPY = nullptr;
	ID3D11ShaderResourceView* _skyboxNY = nullptr;
	ID3D11ShaderResourceView* _skyboxPZ = nullptr;
	ID3D11ShaderResourceView* _skyboxNZ = nullptr;

	float _scale = 100.0f;
	Vector3 _centre;
	XMFLOAT4X4 _world;
public:
	Skybox(MeshData* mesh, ID3D11ShaderResourceView* skyboxPX, ID3D11ShaderResourceView* skyboxNX, ID3D11ShaderResourceView* skyboxPY, ID3D11ShaderResourceView* skyboxNY, ID3D11ShaderResourceView* skyboxPZ, ID3D11ShaderResourceView* skyboxNZ);

	void Update(Vector3 pos);
	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);

};
#endif
