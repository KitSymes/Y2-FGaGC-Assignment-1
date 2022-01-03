#include "Skybox.h"

Skybox::Skybox(MeshData* mesh, ID3D11ShaderResourceView* skyboxPX, ID3D11ShaderResourceView* skyboxNX, ID3D11ShaderResourceView* skyboxPY, ID3D11ShaderResourceView* skyboxNY, ID3D11ShaderResourceView* skyboxPZ, ID3D11ShaderResourceView* skyboxNZ)
{
	_mesh = mesh;
	_skyboxPX = skyboxPX;
	_skyboxNX = skyboxNX;
	_skyboxPY = skyboxPY;
	_skyboxNY = skyboxNY;
	_skyboxPZ = skyboxPZ;
	_skyboxNZ = skyboxNZ;
}

void Skybox::Update(Vector3 pos)
{
	_centre = pos;
	XMStoreFloat4x4(&_world,
		XMMatrixScaling(_scale, _scale, _scale)
		* XMMatrixTranslation(pos.x, pos.y, pos.z));
		//* XMMatrixTranslation(0, 0, 0));
}

void Skybox::Draw(ID3D11DeviceContext* _pImmediateContext, ConstantBuffer* cb, ID3D11Buffer* _pConstantBuffer)
{
	XMMATRIX world = XMLoadFloat4x4(&_world);
	cb->mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, cb, 0, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	// Set Vertex Buffer
	_pImmediateContext->IASetVertexBuffers(0, 1, &_mesh->VertexBuffer, &stride, &offset);
	// Set Index Buffer
	_pImmediateContext->IASetIndexBuffer(_mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxPZ);
	_pImmediateContext->DrawIndexed(3, 0, 0);
	_pImmediateContext->DrawIndexed(3, 18, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxPX);
	_pImmediateContext->DrawIndexed(3, 3, 0);
	_pImmediateContext->DrawIndexed(3, 21, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxNZ);
	_pImmediateContext->DrawIndexed(3, 6, 0);
	_pImmediateContext->DrawIndexed(3, 24, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxNX);
	_pImmediateContext->DrawIndexed(3, 9, 0);
	_pImmediateContext->DrawIndexed(3, 27, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxNY);
	_pImmediateContext->DrawIndexed(3, 12, 0);
	_pImmediateContext->DrawIndexed(3, 30, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_skyboxPY);
	_pImmediateContext->DrawIndexed(3, 15, 0);
	_pImmediateContext->DrawIndexed(3, 33, 0);
}
