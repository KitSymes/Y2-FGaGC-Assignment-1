#include "Player.h"

Player::Player(Geometry* mesh, Vector3 position, Camera* cameraFP, Camera* cameraTP) : GameObject(mesh, position, nullptr)
{
	_cameraFP = cameraFP;
	_cameraTP = cameraTP;

	_up = Vector3(0.0f, 1.0f, 0.0f);
	_forward = Vector3(1.0f, 0.0f, 0.0f);
	_yaw = 0.0f;
	_pitch = 0.0f;
	_firstPerson = true;
}

void Player::Update(float time)
{
	// Rotation
	if (GetAsyncKeyState('Q'))
		SetYaw(_yaw + 0.001f);
	if (GetAsyncKeyState('E'))
		SetYaw(_yaw - 0.001f);

	if (GetAsyncKeyState('R'))
		SetPitch(_pitch + 0.001f);
	if (GetAsyncKeyState('F'))
		SetPitch(_pitch - 0.001f);

	// Movement
	if (GetAsyncKeyState('W'))
		_position += _forward * _speed;
	if (GetAsyncKeyState('S'))
		_position -= _forward * _speed;
	if (GetAsyncKeyState('A'))
		_position += _forward.CrossProduct(_up) * _speed;
	if (GetAsyncKeyState('D'))
		_position -= _forward.CrossProduct(_up) * _speed;

	if (GetAsyncKeyState(VK_SPACE))
		_position.y += _speed;
	if (GetAsyncKeyState(VK_LCONTROL))
		_position.y -= _speed;

	// Switching Perspectives
	if (GetAsyncKeyState('Z'))
		_firstPerson = true;
	if (GetAsyncKeyState('X'))
		_firstPerson = false;

	if (_firstPerson)
	{
		_cameraFP->SetPosition(_position);
		_cameraFP->SetDirection(_forward);
		_cameraFP->SetYaw(_yaw);
		_cameraFP->SetPitch(_pitch);

		_cameraFP->SetView();
		_cameraFP->SetProjection();
		_cameraFP->Update();
	}
	else
	{
		_cameraTP->SetPosition(_position - _forward * 5);
		_cameraTP->SetDirection(_forward);
		_cameraTP->SetYaw(_yaw);
		_cameraTP->SetPitch(_pitch);

		_rotation.y = -_yaw;

		_cameraTP->SetView();
		_cameraTP->SetProjection();
		_cameraTP->Update();
	}

	GameObject::Update(time);
}

void Player::Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer)
{
	if (!_firstPerson)
		GameObject::Draw(_pImmediateContext, cb, _pConstantBuffer);
}

void Player::SetYaw(float yaw)
{
	_yaw = yaw;
	_forward.x = 1 * cos(_yaw) - 0 * sin(_yaw);
	_forward.z = 1 * sin(_yaw) - 0 * cos(_yaw);
}

void Player::SetPitch(float pitch)
{
	if (pitch > M_PI)
		pitch = M_PI;
	else if (pitch < -M_PI)
		pitch = -M_PI;
	_pitch = pitch;
	_forward.y = pitch;
}

XMFLOAT4X4* Player::GetCameraView()
{
	if (_firstPerson)
		return _cameraFP->GetView();
	else
		return _cameraTP->GetView();
}

XMFLOAT4X4* Player::GetCameraProjection()
{
	if (_firstPerson)
		return _cameraFP->GetProjection();
	else
		return _cameraTP->GetProjection();
}
