#include "OrbitGameObject.h"

OrbitGameObject::OrbitGameObject(Geometry* mesh, float speed, Vector3 offset, Vector3 axis, GameObject* target) : GameObject(mesh, offset, target)
{
	_speed = speed;
	_axis = axis;
}

void OrbitGameObject::Update(float time)
{
	XMStoreFloat4x4(&_world,
		XMMatrixScaling(_scale.x, _scale.y, _scale.z)
		* XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z)
		* GetParentRotation(time));
}

XMMATRIX OrbitGameObject::GetParentRotation(float time)
{
	XMMATRIX rotation = XMMatrixTranslation(_position.x, _position.y, _position.z)
		* XMMatrixRotationX(_axis.x * _speed * time) * XMMatrixRotationY(_axis.y * _speed * time) * XMMatrixRotationZ(_axis.z * _speed * time);
	if (_parent != nullptr)
		if (OrbitGameObject* v = dynamic_cast<OrbitGameObject*>(_parent))
			rotation *= v->GetParentRotation(time);
	return rotation;
}
