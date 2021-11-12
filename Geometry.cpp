#include "Geometry.h"

Geometry::Geometry(MeshData* mesh, ID3D11ShaderResourceView* texture)
{
	_mesh = mesh;
	_texture = texture;
}

Geometry::~Geometry()
{
	_mesh = nullptr;
	_texture = nullptr;
}

void Geometry::Draw(ID3D11DeviceContext* _pImmediateContext, ID3D11Buffer* _pConstantBuffer)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	// Set Texture
	//_pImmediateContext->PSSetShaderResources(0, 1, &_texture);

	// Set Vertex Buffer
	_pImmediateContext->IASetVertexBuffers(0, 1, &_mesh->VertexBuffer, &stride, &offset);
	// Set Index Buffer
	_pImmediateContext->IASetIndexBuffer(_mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	/*// Set Vertex Shader
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	// Set Vertex Shader Constant Buffer
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	// Set Pixel Shader
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	// Set Pixel Shader Constant Buffer
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	*/

	// Draw Object
	_pImmediateContext->DrawIndexed(_mesh->IndexCount, 0, 0);
}
