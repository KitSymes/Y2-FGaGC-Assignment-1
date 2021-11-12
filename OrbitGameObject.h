#pragma once
#include "GameObject.h"
class OrbitGameObject :
    public GameObject
{
private:
    GameObject* _target;
    float _speed;
public:
    OrbitGameObject(GameObject* target, Geometry* mesh, float speed, Vector3 position, GameObject* parent);

    void Update(float time);
};

