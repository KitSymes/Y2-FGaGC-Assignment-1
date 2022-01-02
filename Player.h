#pragma once
#ifndef _PLAYER_H
#define _PLAYER_H
#include "GameObject.h"
#include "Camera.h"
class Player :
    public GameObject
{
private:
    Camera* _cameraFP;
    Camera* _cameraTP;

    Vector3 _forward;
    Vector3 _up;

	float _pitch;
	float _yaw;

	bool _firstPerson;
public:
    Player(Geometry* mesh, Vector3 position, Camera* cameraFP, Camera* cameraTP);

    void Update(float time);
	void Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer);

	/// <summary>
	/// Set the Yaw Rotation in radians
	/// </summary>
	/// <param name="yaw">The Yaw value in radians</param>
	void SetYaw(float yaw);
	/// <summary>
	/// Set the Pitch Rotation in radians
	/// </summary>
	/// <param name="pitch">The Pitch value in radians</param>
	void SetPitch(float pitch);
	XMFLOAT4X4* GetCameraView();
	XMFLOAT4X4* GetCameraProjection();
};
#endif
