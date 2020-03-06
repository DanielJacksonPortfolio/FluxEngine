#pragma once
#include "../../Tools/StrippedWindows.h"
#include "Model Data/Model.h"
#include <DirectXMath.h>
using namespace DirectX;

class Appearance
{
public:
	Appearance() {}
	Appearance(const Appearance& app);
	Model& GetModel() { return this->model; }
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
	Model model = Model();

	std::string filepath = "";
	bool wireframe = false;
	int grayscale = 0;
	bool normalMap = false;
	bool specularMap = false;
	bool renderMode = true;
};

