#include "Camera.h"
#include <sstream>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}


Camera::Camera(Vector3 pos, Vector3 at, Vector3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearPlane, FLOAT farPlane, bool lookTo)
{
	_eye = pos;
	_at = at;
	_up = up;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
	_lookTo = lookTo;
	//_direction = Vector3(1.0f, 0.0f, 0.0f);
	_direction = at;
	_rotationYaw = 0.0f;
	_rotationPitch = 0.0f;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// E1 + E2 Camera 1 is a _lookTo camera, meaning it can roatate and move based on its rotation
	if (_lookTo)
	{
		if (GetAsyncKeyState('Q'))
			SetYaw(_rotationYaw + 0.001f);
		if (GetAsyncKeyState('E'))
			SetYaw(_rotationYaw - 0.001f);

		if (GetAsyncKeyState('R'))
			SetPitch(_rotationPitch + 0.001f);
		if (GetAsyncKeyState('F'))
			SetPitch(_rotationPitch - 0.001f);

		//DBOUT("" << _direction.x << " " << _direction.z << std::endl);

		// Movement
		if (GetAsyncKeyState('W'))
			_eye += _direction * 0.001f;
		if (GetAsyncKeyState('S'))
			_eye -= _direction * 0.001f;
		if (GetAsyncKeyState('A'))
			_eye += _direction.CrossProduct(_up) * 0.001f;
		if (GetAsyncKeyState('D'))
			_eye -= _direction.CrossProduct(_up) * 0.001f;

		if (GetAsyncKeyState(VK_SPACE))
			_eye.y += 0.001;
		if (GetAsyncKeyState(VK_LCONTROL))
			_eye.y -= 0.001;

	}
}

void Camera::SetPosition(Vector3 pos)
{
	_eye = pos;
}

Vector3 Camera::GetPosition()
{
	return _eye;
}

void Camera::SetUp(Vector3 up)
{
	_up = up;
}

Vector3 Camera::GetUp()
{
	return _up;
}

XMFLOAT4X4* Camera::GetView()
{
	return &_view;
}

void Camera::SetView()
{
	XMVECTOR Eye = XMVectorSet(_eye.x, _eye.y, _eye.z, 0.0f);
	XMVECTOR Up = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);
	XMVECTOR Direction = XMVectorSet(_direction.x, _direction.y, _direction.z, 0.0f);
	if (_lookTo)
	{
		XMStoreFloat4x4(&_view, XMMatrixLookToLH(Eye, Direction, Up));
	}
	else
		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, Direction, Up));
}

XMFLOAT4X4* Camera::GetProjection()
{
	return &_projection;
}

void Camera::SetProjection()
{
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, _nearPlane, _farPlane));
}

void Camera::SetDirection(Vector3 dir)
{
	_direction = dir;
}

void Camera::SetYaw(float yaw)
{
	_rotationYaw = yaw;
	_direction.x = 1 * cos(_rotationYaw) - 0 * sin(_rotationYaw);
	_direction.z = 1 * sin(_rotationYaw) - 0 * cos(_rotationYaw);
}

void Camera::SetPitch(float pitch)
{
	if (pitch > M_PI)
		pitch = M_PI;
	else if (pitch < -M_PI)
		pitch = -M_PI;
	_rotationPitch = pitch;
	_direction.y = _rotationPitch;
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearPlane, FLOAT farPlane, bool lookTo)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
	_lookTo = lookTo;
}
