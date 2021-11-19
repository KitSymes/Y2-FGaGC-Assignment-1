#include "Camera.h"

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
}

Camera::~Camera()
{
}

void Camera::Update()
{
}

void Camera::SetPosition(Vector3 pos)
{
	_eye = pos;
}

Vector3 Camera::GetPosition()
{
	return _eye;
}

void Camera::SetLookAt(Vector3 at)
{
	_at = at;
}

Vector3 Camera::GetLookAt()
{
	return _at;
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
	XMVECTOR At = XMVectorSet(_at.x, _at.y, _at.z, 0.0f);
	XMVECTOR Up = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
}

XMFLOAT4X4* Camera::GetProjection()
{
	return &_projection;
}

void Camera::SetProjection()
{
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / (FLOAT)_windowHeight, 0.01f, 100.0f));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearPlane, FLOAT farPlane, bool lookTo)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
	_lookTo = lookTo;
}
