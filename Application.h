#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Vector3.h"
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "OrbitGameObject.h"
#include "Camera.h"
#include "Terrain.h"
#include "Player.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	// W02 Depth/Stencil Buffer
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	// W02 Render States
	bool					_showWireFrame = false;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidFrame;
	// W02 Planet Positions
	XMFLOAT4X4 _planet2World;
	XMFLOAT4X4 _moon2World;
	Vector3					_meteorsOffsets[100];
	XMFLOAT4X4				_meteorsWorlds[100];
	// W03 Shader Pipeline
	float _time;
	ID3D11Buffer* _pPyramidVertexBuffer;
	ID3D11Buffer* _pPyramidIndexBuffer;
	// W04 Lighting
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 EyePosW;
	// W05 Post Processing
	ID3D11ShaderResourceView* _crateTexture = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;
	// W06 Game Architecture
	MeshData _sphereMeshData;
	MeshData _cubeMeshData;
	MeshData _planeMeshData;
	Geometry* _sunGeo;
	Geometry* _planetGeo;
	Geometry* _planeGeo;
	GameObject* _sun;
	GameObject* _planet1;
	GameObject* _moon1;
	GameObject* _moon2;
	ID3D11ShaderResourceView* _planeTexture = nullptr;
	// W07 Camera Control
	Camera* _camera;
	Camera* _camera1;
	Camera* _camera2;
	Camera* _camera3;
	Camera* _camera4;
	// W08 Transparency
	ID3D11BlendState* _transparency;
	// W10 Terrain
	Terrain* _terrain;
	//
	Player* _player;
	Camera* _cameraFP;
	Camera* _cameraTP;
	bool _playerEnabled;
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

