#include "Shader.h"

ID3D10Blob* Shader::GetBuffer()
{
	return this->shaderBuffer.Get();
}