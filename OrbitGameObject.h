#pragma once
#include "GameObject.h"
class OrbitGameObject :
    public GameObject
{
private:
    //GameObject* _target;
    float _speed;
    Vector3 _axis;
public:
    OrbitGameObject(Geometry* mesh, float speed, Vector3 offset, Vector3 axis, GameObject* target);

    void Update(float time);

    XMMATRIX GetParentRotation(float time);
};

