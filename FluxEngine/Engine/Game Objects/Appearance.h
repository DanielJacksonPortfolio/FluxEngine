#pragma once
#include "../../Tools/StrippedWindows.h"
#include "Model Data/Model.h"
#include <DirectXMath.h>
using namespace DirectX;

class Appearance
{
public:
	Appearance() 
	{
		model = new Model();
	}
	Appearance(const Appearance& app);
	Model* GetModel() { return this->model; }

	Model::BoundingShape GetBoundingShape() { return this->model->GetBoundingShape(); }
	float GetVolume() { return this->model->GetVolume(); }
	XMVECTOR GetOriginVector() { return this->model->GetOriginVector(); }
	float GetScaledBoundingSphereRadius() { return model->GetScaledBoundingSphereRadius(); }
	void SetScaledBoundingSphereRadius(float scale) { model->SetScaledBoundingSphereRadius(scale); }
	float GetBoundingSphereRadius() { return model->GetBoundingSphereRadius(); }
	XMVECTOR GetMinimumCoord(XMMATRIX rotationMatrix = XMMATRIX()) {return model->GetMinimumCoord(rotationMatrix);}
	XMVECTOR GetMaximumCoord(XMMATRIX rotationMatrix = XMMATRIX()) {return model->GetMaximumCoord(rotationMatrix);}
	float GetWidth(XMMATRIX rotationMatrix = XMMATRIX()) { return model->GetWidth(rotationMatrix); }
	float GetHeight(XMMATRIX rotationMatrix = XMMATRIX()) { return model->GetHeight(rotationMatrix); }
	float GetDepth(XMMATRIX rotationMatrix = XMMATRIX()) { return model->GetDepth(rotationMatrix); }
	int& GetGrayscale() { return this->grayscale; }
	void SetGrayscale(int grayscale) { this->grayscale = grayscale; }
	bool& GetNormalMapMode() { return this->normalMap; }
	void SetNormalMapMode(bool normalMap) { this->normalMap = normalMap; }
	bool& GetSpecularMapMode() { return this->specularMap; }
	void SetSpecularMapMode(bool specularMap) { this->specularMap = specularMap; }
	bool& GetWireframeMode() { return this->wireframe; }
	void SetWireframeMode(bool wireframe) { this->wireframe = wireframe; }
	bool& GetRenderMode() { return this->renderMode; }
	void SetRenderMode(bool mode) { this->renderMode = mode; }
	std::string GetFilepath() { return this->filepath; }
	void SetFilepath(std::string filepath) { this->filepath = filepath; }
private:
	Model* model = nullptr;

	std::string filepath = "";
	bool wireframe = false;
	int grayscale = 0;
	bool normalMap = false;
	bool specularMap = false;
	bool renderMode = true;
};

