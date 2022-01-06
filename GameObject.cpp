#include "GameObject.h"

GameObject::GameObject(Geometry* mesh, Vector3 position, GameObject* parent)
{
	_mesh = mesh;
	_position = position;
	_scale = Vector3(1, 1, 1);
	_rotation = Vector3();
	_parent = parent;
}

GameObject::~GameObject()
{

}

void GameObject::Update(float time)
{
	Vector3 offset = GetOffset();
	XMStoreFloat4x4(&_world,
		XMMatrixScaling(_scale.x, _scale.y, _scale.z)
		* XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z)
		* XMMatrixTranslation(offset.x, offset.y, offset.z));
}

void GameObject::Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer)
{
	XMMATRIX world = XMLoadFloat4x4(&_world);
	cb->mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, cb, 0, 0);

	if (_mesh != nullptr)
		_mesh->Draw(_pImmediateContext, _pConstantBuffer);
}

Vector3 GameObject::GetOffset()
{
	Vector3 offset = _position;
	if (_parent != nullptr)
		offset += _parent->GetOffset();
	return offset;
}
