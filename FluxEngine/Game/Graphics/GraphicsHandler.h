#pragma once

//#include <SpriteBatch.h>
//#include <SpriteFont.h>
#include <memory>
#include <WICTextureLoader.h>
#include "AdapterHandler.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "../../Tools/Timer.h"
#include "../Resources/Config.h"

#include "VertexTypes.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

//#include "Light.h"
//#include "Camera.h"
//#include "ImGUI\\imgui.h"
//#include "ImGUI\\imgui_impl_win32.h"
//#include "ImGUI\\imgui_impl_dx11.h"

class GraphicsHandler
{
public:

	bool Init(HWND hWnd, int width, int height, Config* config);
	void RenderFrame();
	~GraphicsHandler();
	bool wireframe = false;
private:
	bool InitDirectX();
	bool InitShaders();
	bool InitScene();
	//void InitImGUI(HWND hWnd);

	int windowWidth, windowHeight = 0;
	Timer fpsTimer;
	HWND hWnd = NULL;
	Config* config = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexBuffer<Vertex_PC> vertexBuffer;
	IndexBuffer indexBuffer;

	VertexShader vertexShader;
	PixelShader pixelShader;

	//ConstantBuffer<CB_VS_vertexShader> cb_vs_vertexShader;
	//ConstantBuffer<CB_PS_light> cb_ps_light;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_WireFrame;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	//std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	//std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

