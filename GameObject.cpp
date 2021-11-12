#include "GameObject.h"

GameObject::GameObject(Geometry* mesh, Vector3 position)
{
	_mesh = mesh;
	_position = position;
	_scale = Vector3(1, 1, 1);
	_rotation = Vector3();
}

GameObject::~GameObject()
{
	delete _mesh;
	_mesh = nullptr;
}

void GameObject::Update(float t)
{
	// TODO make function to get parent position, rotation and scale and recurse
	XMStoreFloat4x4(&_world,
		//XMMatrixScaling(_scale.x, _scale.y, _scale.z)
		//* XMMatrixRotationX(_rotation.x) * XMMatrixRotationZ(_rotation.y) * XMMatrixRotationZ(_rotation.z)
		//* XMMatrixTranslation(_position.x, _position.y, _position.z));
		XMMatrixRotationY(t / 4));
}

void GameObject::Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer)
{
	XMMATRIX world = XMLoadFloat4x4(&_world);
	cb->mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, cb, 0, 0);

	_mesh->Draw(_pImmediateContext, _pConstantBuffer);
}
