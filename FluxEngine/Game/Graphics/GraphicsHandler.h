#pragma once

#include "AdapterHandler.h"
#include "Shaders+Buffers/VertexShader.h"
#include "Shaders+Buffers/PixelShader.h"
#include "../../Tools/Timer.h"
#include "../Resources/Config.h"

#include "Objects/Camera.h"
#include "Objects/RenderableGameObject.h"

//#include "Light.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"

#include <memory>
#include <SpriteBatch.h>
#include <SpriteFont.h>

class GraphicsHandler
{
public:

	bool Init(HWND hWnd, int width, int height, Config* config);
	void RenderFrame();
	bool wireframe = false;
	float cameraSpeed = 0.005f;
	float bgColor[4] = { 0.1f,0.1f,0.1f,1.0f };
	Camera camera;
	RenderableGameObject cube;
private:
	bool InitDirectX();
	bool InitShaders();
	bool InitScene();
	void InitImGUI();
	void RenderGUI();

	int windowWidth, windowHeight = 0;
	Timer fpsTimer;
	HWND hWnd = NULL;
	std::unique_ptr<Config> config = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_WireFrame;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront_WireFrame;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	VertexShader vertexShader;
	PixelShader pixelShader;

	ConstantBuffer<CB_vertexShader_PosTex> cb_vertexShader;
};

