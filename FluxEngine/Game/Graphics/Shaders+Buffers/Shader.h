#pragma once
#include "../../../Tools/ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class Shader
{
public:
	ID3D10Blob* GetBuffer();
protected:
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};