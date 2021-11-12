#pragma once

#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H
#include <d3d11_1.h>
#include "Structures.h"
#include "Vector3.h"
#include "Geometry.h"

class GameObject
{
private:
public:
	Geometry* _mesh;
	Vector3 _position;
	Vector3 _rotation;
	Vector3 _scale;

	XMFLOAT4X4 _world;
	GameObject(Geometry* mesh, Vector3 position = Vector3());
	~GameObject();

	void Update(float t);
	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);
};
#endif
