#pragma once

#ifndef _CAMERA_H
#define _CAMERA_H
#include <directxmath.h>
#include <directxcolors.h>
#include <d3d11_1.h>
#include "Vector3.h"

using namespace DirectX;

class Camera
{
private:
	Vector3 _eye;
	Vector3 _at;
	Vector3 _up;
	Vector3 _direction;
	float _rotationYaw;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearPlane;
	FLOAT _farPlane;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	bool _lookTo;
public:
	Camera(Vector3 pos, Vector3 at, Vector3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearPlane, FLOAT farPlane, bool lookTo);
	~Camera();

	void Update();

	void SetPosition(Vector3 pos);
	Vector3 GetPosition();
	void SetLookAt(Vector3 at);
	Vector3 GetLookAt();
	void SetUp(Vector3 up);
	Vector3 GetUp();

	XMFLOAT4X4* GetView();
	void SetView();
	XMFLOAT4X4* GetProjection();
	void SetProjection();

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearPlane, FLOAT farPlane, bool lookTo);
};
#endif
