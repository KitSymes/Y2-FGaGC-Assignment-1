#include "Application.h"
#include <time.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	/*_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;*/
	_pPyramidVertexBuffer = nullptr;
	_pPyramidIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	srand(time(NULL));

	// Light direction from surface (XYZ)
	lightDirection = XMFLOAT3(0.25f, 0.5f, 1.0f);
	//lightDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// Diffuse material properties (RGBA)
	diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
	// Diffuse light colour (RGBA)
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Ambient material properties (RGBA)
	ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	// Ambient light colour (RGBA)
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0);

	// Specular material properties (RGBA)
	specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	// Specular light colour (RGBA)
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	specularPower = 2.0;

	//specularMaterial = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//specularLight = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	EyePosW = XMFLOAT3(0.0f, 4.0f, -8.0f);

	for (int i = 0; i < 100; i++)
	{
		_meteorsOffsets[i].x = (rand() % 80 - 40) / 10.0f;
		if (_meteorsOffsets[i].x > 0 && _meteorsOffsets[i].x < 1.5f)
			_meteorsOffsets[i].x += 1.5f;
		_meteorsOffsets[i].y = (rand() % 40 - 20) / 10.0f;
		_meteorsOffsets[i].z = (rand() % 80 - 40) / 10.0f;
		if (_meteorsOffsets[i].z > 0 && _meteorsOffsets[i].z < 1.5f)
			_meteorsOffsets[i].z += 1.5f;
	}

	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	/*// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -8.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));*/
	_camera1 = new Camera(Vector3(0.0f, 4.0f, -8.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.0f, 500.0f, true);
	_camera1->SetYaw(1.57079633f);
	//_camera1 = new Camera(Vector3(0.0f, 1.0f, 0.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.0f, 100.0f, true);
	_camera2 = new Camera(Vector3(-8.0f, 4.0f, 0.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.0f, 500.0f, false);
	_camera3 = new Camera(Vector3(0.0f, 4.0f, 8.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.0f, 500.0f, false);
	_camera4 = new Camera(Vector3(8.0f, 4.0f, 0.0f), Vector3(), Vector3(0.0f, 1.0f, 0.0f), _WindowWidth, _WindowHeight, 0.0f, 500.0f, false);
	_camera = _camera1;
	_camera->SetView();
	_camera->SetProjection();

	// OBJ Init Stuff - The false at the end is "wether to flip the texture co-ords or not", false for Blender and not needed for 3DS Max
	_sphereMeshData = OBJLoader::Load("sphere.obj", _pd3dDevice, false);
	_cubeMeshData = OBJLoader::Load("cube.obj", _pd3dDevice, false);
	_planeMeshData = OBJLoader::Load("plane.obj", _pd3dDevice, false);

	_sunGeo = new Geometry(&_sphereMeshData, _crateTexture);
	_sun = new GameObject(_sunGeo, Vector3(), nullptr);

	_planetGeo = new Geometry(&_cubeMeshData, _crateTexture);
	_planet1 = new OrbitGameObject(_planetGeo, (float)2 / 3, Vector3(4.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), _sun);
	_planet1->_scale = Vector3(0.5f, 0.5f, 0.5f);

	_planeGeo = new Geometry(&_planeMeshData, _planeTexture);
	_moon1 = new OrbitGameObject(_planeGeo, 2, Vector3(0.0f, 1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), _planet1);
	_moon1->_scale = Vector3(0.05f, 0.05f, 0.05f);
	_moon2 = new OrbitGameObject(_planeGeo, 2, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), _planet1);
	_moon2->_scale = Vector3(0.05f, 0.05f, 0.05f);
	_moon2->_rotation = Vector3(90.0f, 90.0f, 0.0f);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

	/*{
		// Create vertex buffer
		SimpleVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT3(0.5773f, -0.5773f, 0.5773f),		XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f),    XMFLOAT3(-0.5773f, 0.5773f, -0.5773f),	XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f),  XMFLOAT3(-0.5773f, 0.5773f, 0.5773f),		XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f),   XMFLOAT3(0.5773f, 0.5773f, 0.5773f),		XMFLOAT2(1.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, 1.0f, 1.0f),	 XMFLOAT3(-0.5773f, -0.5773f, -0.5773f),	XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f),    XMFLOAT3(0.5773f, 0.5773f, -0.5773f),		XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f),  XMFLOAT3(0.5773f, -0.5773f, -0.5773f),		XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT3(-0.5773f, -0.5773f, 0.5773f),		XMFLOAT2(1.0f, 1.0f) },
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * 8;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;

		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

		if (FAILED(hr))
			return hr;
	}*/

	{
		// Create pyramid vertex buffer
		SimpleVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT3(-0.6626f, -0.3490f, -0.6626f),	XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT3(-0.6626f, -0.3490f, 0.6626f),	XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f),		XMFLOAT3(0.6626f, -0.3490f, -0.6626f),	XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f),		XMFLOAT3(0.6626f, -0.3490f, 0.6626f),	XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT3(0.0f, 1.0f, 0.0f),				XMFLOAT2(0.5f, 0.5f) },
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * 5;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;

		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidVertexBuffer);

		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

	/*{
		// Create index buffer
		WORD indices[] =
		{
			// Front Face
			0,1,2,
			2,1,3,
			// Top Face
			0,4,1,
			1,4,5,
			// Back Face
			6,5,4,
			7,5,6,
			// Bottom Face
			3,6,2,
			7,6,3,
			// Right
			1,5,3,
			3,5,7,
			//
			0,6,4,
			0,2,6,
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * 6 * 6;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indices;
		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

		if (FAILED(hr))
			return hr;
	}*/

	{
		// Create Pyramid index buffer
		WORD indices[] =
		{
			// Bottom Face
			0,1,2,
			1,3,2,
			// Side Faces
			0,4,1,
			1,4,3,
			3,4,2,
			2,4,0,
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * 6 * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indices;
		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPyramidIndexBuffer);

		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		if (pErrorBlob) pErrorBlob->Release();

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	// Define the Depth/Stencil Buffer (Sem1W02)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// Create the Depth/Stencil Buffer
	hr = _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_WindowWidth;
	vp.Height = (FLOAT)_WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();


	InitIndexBuffer();

	_terrain = new Terrain();
	_terrain->GenerateGrid(99, 99, _pd3dDevice);

	/*// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
	// Set index buffer
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);*/

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	// Render States (Sem1W02)
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	if (FAILED(hr))
		return hr;

	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_solidFrame);

	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_crateTexture);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->PSSetShaderResources(0, 1, &_crateTexture);


	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Plane_COLOR.dds", nullptr, &_planeTexture);

	if (FAILED(hr))
		return hr;

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	hr = _pd3dDevice->CreateBlendState(&blendDesc, &_transparency);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Application::Cleanup()
{
	if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_solidFrame) _solidFrame->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_depthStencilView) _depthStencilView->Release();

	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_pConstantBuffer) _pConstantBuffer->Release();
	//if (_pVertexBuffer) _pVertexBuffer->Release();
	//if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPyramidVertexBuffer) _pPyramidVertexBuffer->Release();
	if (_pPyramidIndexBuffer) _pPyramidIndexBuffer->Release();
	delete _terrain;
	_terrain = nullptr;
	if (_pVertexLayout) _pVertexLayout->Release();
	if (_pVertexShader) _pVertexShader->Release();
	if (_pPixelShader) _pPixelShader->Release();
	if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pSwapChain) _pSwapChain->Release();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pd3dDevice) _pd3dDevice->Release();
	// W06
	delete _sunGeo;
	_sunGeo = nullptr;
	delete _planetGeo;
	_planetGeo = nullptr;
	delete _planeGeo;
	_planeGeo = nullptr;
	delete _sun;
	_sun = nullptr;
	delete _planet1;
	_planet1 = nullptr;
	delete _moon1;
	_moon1 = nullptr;
	delete _moon2;
	_moon2 = nullptr;
	if (_planeTexture) _planeTexture->Release();
	// W07
	delete _camera1;
	delete _camera2;
	delete _camera3;
	delete _camera4;
	_camera = nullptr;
	_camera1 = nullptr;
	_camera2 = nullptr;
	_camera3 = nullptr;
	_camera4 = nullptr;
	// W08
	if (_transparency) _transparency->Release();

	_camera = nullptr;
	delete _camera1;
	delete _camera2;
	delete _camera3;
	delete _camera4;

	//if (_sun)
	//	delete _sun;
	//if (_sunGeo)
	//	delete _sunGeo;
	//if (_planet1)
	//	delete _planet1;
	//if (_planetGeo)
	//	delete _planetGeo;
}

void Application::Update()
{
	// Update our time
	static float t = 0.0f;
	_time = t;

	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	//
	// Animate the cubes
	//

	// Sun
	XMStoreFloat4x4(&_world, XMMatrixRotationY(t / 4));

	/*// Planet 1
	// Planet Spin Z * Planet Spin X * Planet Scale * Planet Move * Panet Orbit Y
	XMStoreFloat4x4(&_planet1World, XMMatrixRotationZ(t) * XMMatrixRotationX(t) * // Spin around itself
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(4, 0, 0) * XMMatrixRotationY(t / 1.5f)); // Scale, Move then Orbit*/
		// Planet2
	XMStoreFloat4x4(&_planet2World, XMMatrixRotationZ(t) * XMMatrixRotationX(t) * // Spin around itself
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(7, 0, 0) * XMMatrixRotationY(t / 2.5f)); // Scale, Move then Orbit

	// Moon 1
	/*XMStoreFloat4x4(&_moon1World, XMMatrixRotationZ(t * 1.5f) * XMMatrixRotationX(t * 1.5f) * // Spin around itself
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0, 4, 0) * XMMatrixRotationX(t * 2) *
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(4, 0, 0) * XMMatrixRotationY(t / 1.5f)); // Planet 1 Transformations*/
		// Moon 2
	XMStoreFloat4x4(&_moon2World, XMMatrixRotationZ(t * 1.5f) * XMMatrixRotationX(t * 1.5f) * // Spin around itself
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0, 3, 0) * XMMatrixRotationZ(t * 2.5f) *
		XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(4, 0, 0) * XMMatrixRotationY(t / 1.5f)); // Planet 1 Transformations

	// Asteroids
	for (int i = 0; i < 100; i++)
	{
		XMStoreFloat4x4(&_meteorsWorlds[i], XMMatrixRotationZ(t * 2.0f) * XMMatrixRotationX(t * 2.0f) * // Spin around itself
			XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslation(_meteorsOffsets[i].x, _meteorsOffsets[i].y, _meteorsOffsets[i].z) * XMMatrixRotationY(t * 2.5f) *
			XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(7, 0, 0) * XMMatrixRotationY(t / 2.5f)); // Planet 2 Transformations
	}

	_terrain->Update();

	_sun->Update(t);
	_planet1->Update(t);
	_moon1->Update(t);
	_moon2->Update(t);

	if (GetAsyncKeyState('1'))
		_camera = _camera1;
	else if (GetAsyncKeyState('2'))
		_camera = _camera2;
	else if (GetAsyncKeyState('3'))
		_camera = _camera3;
	else if (GetAsyncKeyState('4'))
		_camera = _camera4;

	_camera->SetView();
	_camera->SetProjection();
	_camera->Update();


	_showWireFrame = GetAsyncKeyState(VK_LSHIFT);
}

void Application::Draw()
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(_camera->GetView());
	XMMATRIX projection = XMLoadFloat4x4(_camera->GetProjection());

	//
	// Update variables
	//
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.DiffuseMtrl = diffuseMaterial;
	cb.DiffuseLight = diffuseLight;
	cb.LightVecW = lightDirection;
	cb.AmbientMtrl = ambientMaterial;
	cb.AmbientLight = ambientLight;
	cb.SpecularMtrl = specularMaterial;
	cb.SpecularLight = specularLight;
	cb.SpecularPower = specularPower;
	cb.EyePosW = EyePosW;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	if (_showWireFrame)
		_pImmediateContext->RSSetState(_wireFrame);
	else
		_pImmediateContext->RSSetState(_solidFrame);

	int pyramidIndexCount = 3 * 6;

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);


	// "fine-tune" the blending equation
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	// Render Opaque Objects //

	_sun->Draw(_pImmediateContext, &cb, _pConstantBuffer);
	_planet1->Draw(_pImmediateContext, &cb, _pConstantBuffer);

	// Set to Cube
	_pImmediateContext->IASetVertexBuffers(0, 1, &_cubeMeshData.VertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_cubeMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Second Planet
	world = XMLoadFloat4x4(&_planet2World);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(_cubeMeshData.IndexCount, 0, 0);

	_moon1->Draw(_pImmediateContext, &cb, _pConstantBuffer);
	_moon2->Draw(_pImmediateContext, &cb, _pConstantBuffer);
	
	_pImmediateContext->PSSetShaderResources(0, 1, &_crateTexture);
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_terrain->Draw(_pImmediateContext, &cb, _pConstantBuffer);
	
	// Set the blend state for transparent objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);

	// Render Transparent Objects //

	// Set to Pyramid
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Asteroid Belt
	for (int i = 0; i < 100; i++)
	{
		world = XMLoadFloat4x4(&_meteorsWorlds[i]);
		cb.mWorld = XMMatrixTranspose(world);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		_pImmediateContext->DrawIndexed(pyramidIndexCount, 0, 0);
	}

	// Present our back buffer to our front buffer
	_pSwapChain->Present(0, 0);
}