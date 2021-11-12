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

using namespace DirectX;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;
	float p1;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	XMFLOAT3 EyePosW;
	float SpecularPower;
};

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
	//ID3D11Buffer*           _pVertexBuffer;
	//ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	// W02 Depth/Stencil Buffer
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	// W02 Render States
	bool					_showWireFrame = false;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidFrame;
	// W02 Planet Positions
	XMFLOAT4X4 _planet1World, _planet2World, _moon1World, _moon2World;
	Vector3					_meteorsOffsets[100];
	XMFLOAT4X4				_meteorsWorlds[100];
	// W03
	float _time;
	ID3D11Buffer* _pPyramidVertexBuffer;
	ID3D11Buffer* _pPyramidIndexBuffer;
	ID3D11Buffer* _pGridVertexBuffer;
	ID3D11Buffer* _pGridIndexBuffer;
	XMFLOAT4X4 _gridWorld;
	int _gridWidth, _gridHeight;
	HRESULT GenerateGrid(int width, int height);
	// W04
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 EyePosW;
	// W05
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;
	// W06
	MeshData _sphereMeshData;
	MeshData _cubeMeshData;
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

