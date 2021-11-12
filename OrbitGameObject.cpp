#include "OrbitGameObject.h"

OrbitGameObject::OrbitGameObject(GameObject* target, Geometry* mesh, float speed, Vector3 position, GameObject* parent) : GameObject(mesh, position, parent)
{
	_target = target;
	_speed = speed;
}

void OrbitGameObject::Update(float time)
{
	Vector3 offset = GetOffset();
	XMStoreFloat4x4(&_world,
		XMMatrixScaling(_scale.x, _scale.y, _scale.z)
		* XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z)
		* XMMatrixTranslation(offset.x, offset.y, offset.z)
		* XMMatrixRotationY(time * _speed));
}
