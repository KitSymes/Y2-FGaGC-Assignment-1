#pragma once

#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H
#include <d3d11_1.h>
#include "Structures.h"
#include "Vector3.h"
#include "Geometry.h"

class GameObject
{
protected:
	Geometry* _mesh;
	XMFLOAT4X4 _world;
	GameObject* _parent;

	Vector3 GetOffset();
public:
	Vector3 _position;
	Vector3 _rotation;
	Vector3 _scale;

	GameObject(Geometry* mesh, Vector3 position, GameObject* parent);
	virtual ~GameObject();

	virtual void Update(float time);
	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);
};
#endif
