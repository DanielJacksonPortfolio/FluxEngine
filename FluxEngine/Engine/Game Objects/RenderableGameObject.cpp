#include "RenderableGameObject.h"

bool RenderableGameObject::Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	if(!Init(data[1], std::stof(data[2]), device, deviceContext, cb_vertexShader, cb_pixelShader, data[0]))
		return false;
	SetPosition(std::stof(data[3]), std::stof(data[4]), std::stof(data[5]));
	SetRotation(std::stof(data[6]), std::stof(data[7]), std::stof(data[8]));
	SetLockedPos(std::stoi(data[9]), 0);
	SetLockedPos(std::stoi(data[10]), 1);
	SetLockedPos(std::stoi(data[11]), 2);
	SetLockedRot(std::stoi(data[12]), 0);
	SetLockedRot(std::stoi(data[13]), 1);
	SetLockedRot(std::stoi(data[14]), 2);
	SetMovable(std::stoi(data[15]));
	SetWireframeMode(std::stoi(data[16]));
	SetGrayscale(std::stoi(data[17]));
	SetNormalMapMode(std::stoi(data[18]));
	SetSpecularMapMode(std::stoi(data[19]));
	SetScale(std::stof(data[20]));
	SetRenderMode(std::stof(data[21]));
	return true;
}

RenderableGameObject::RenderableGameObject(const RenderableGameObject& obj)
{
	Model model;
	XMMATRIX worldMatrix = XMMatrixIdentity();
	bool movable = false;
	bool wireframe = false;
	int grayscale = 0;
	bool normalMap = false;
	bool specularMap = false;
	bool renderMode = true;
	float initScale = 1.0f;
	float scale = 1.0f;
	std::string filepath;

	this->model = obj.model;
	this->worldMatrix = obj.worldMatrix;
	this->movable = obj.movable;
	this->wireframe = obj.wireframe;
	this->grayscale = obj.grayscale;
	this->normalMap = obj.normalMap;
	this->specularMap = obj.specularMap;
	this->renderMode = obj.renderMode;
	this->initScale = obj.initScale;
	this->scale = obj.scale;
	this->filepath = obj.filepath;
}

std::string RenderableGameObject::Save()
{
	std::string output = "";
	output += objectName + ",";
	output += filepath + ",";
	output += std::to_string(initScale) + ",";
	output += std::to_string(pos.x) + ",";
	output += std::to_string(pos.y) + ",";
	output += std::to_string(pos.z) + ",";
	output += std::to_string(rot.x) + ",";
	output += std::to_string(rot.y) + ",";
	output += std::to_string(rot.z) + ",";
	output += std::to_string(lockedPos[0]) + ",";
	output += std::to_string(lockedPos[1]) + ",";
	output += std::to_string(lockedPos[2]) + ",";
	output += std::to_string(lockedRot[0]) + ",";
	output += std::to_string(lockedRot[1]) + ",";
	output += std::to_string(lockedRot[2]) + ",";
	output += std::to_string(movable) + ",";
	output += std::to_string(wireframe) + ",";
	output += std::to_string(grayscale) + ",";
	output += std::to_string(normalMap) + ",";
	output += std::to_string(specularMap) + ",";
	output += std::to_string(scale)+",";
	output += std::to_string(renderMode);
	return output;
}

bool RenderableGameObject::Init(const std::string& filepath, float scale, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name)
{
	scale = scale > 0 ? scale : 0.01f;
	this->filepath = filepath;
	if (name != "")
		objectName = name;
	if (!model.Init(filepath, scale, device, deviceContext, cb_vertexShader, cb_pixelShader))
	{
		ErrorLogger::Log("Failed to load model @ " + filepath);
		return false;
	}
	this->initScale = scale;
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	if(renderMode)
		model.Draw(this->worldMatrix, viewProjectionMatrix);
}


void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(scale,scale,scale) * XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}