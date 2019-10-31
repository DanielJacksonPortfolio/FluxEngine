#include "RenderableGameObject.h"

bool RenderableGameObject::Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader_PosTex>& cb_vertexShader)
{
	if (!model.Init(filepath, device, deviceContext, cb_vertexShader))
		return false;
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}