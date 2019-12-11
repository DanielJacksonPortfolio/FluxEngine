#pragma once

#include "AdapterHandler.h"
#include "Shaders+Buffers/VertexShader.h"
#include "Shaders+Buffers/PixelShader.h"
#include "../../Tools/Timer.h"
#include "../Resources/Config.h"

#include "Objects/Camera.h"
#include "Objects/RenderableGameObject.h"
#include "Objects/PointLight.h"
#include "Objects/DirectionalLight.h"

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_win32.h>
#include <ImGUI/imgui_impl_dx11.h>

#include <memory>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <unordered_map>

#include "Bindables/RasterizerState.h"

class GraphicsHandler
{
public:

	bool Init(HWND hWnd, int width, int height, Config* config);
	void RenderFrame();
	Camera* camera;
	RenderableGameObject* currentObject;
	RenderableGameObject* skybox;

	PointLight* pointLight;
	DirectionalLight* directionalLight;

	std::unordered_map<std::string, Bindable*> bindables = {};
private:
	bool InitDirectX();
	bool InitShaders();
	bool InitScene();
	void InitImGUI();

	void RenderGUI();

	void NextCamera(int direction = 1);
	void NextPLight(int direction = 1);
	void NextDLight(int direction = 1);
	void NextObject(int direction = 1);

	void NewCamera();
	void NewPointLight();
	void NewDirectionalLight();
	void NewObject();

	void DuplicateCamera();
	void DuplicatePointLight();
	void DuplicateDirectionalLight();
	void DuplicateObject();

	void DeleteCamera();
	void DeletePointLight();
	void DeleteDirectionalLight();
	void DeleteObject();

	void DeleteCameras();
	void DeletePointLights();
	void DeleteDirectionalLights();
	void DeleteObjects();

	void LoadScene(std::string sceneName);
	void SaveScene(std::string sceneName);

	std::vector<Camera*> cameras = {};
	std::vector<PointLight*> pLights = {};
	std::vector<DirectionalLight*> dLights = {};
	std::unordered_map<std::string, RenderableGameObject*> objects = {};

	int cameraID = 0;
	int pLightID = 0;
	int dLightID = 0;
	int objectID = 0;

	bool showLights = true;
	bool showPLControls = true;
	bool showDLControls = true;
	bool showCamControls = true;
	bool showObjectControls = true;
	bool showGeneralControls = true;
	bool showNewObjectWindow = false;

	float shininess = 8.0f;
	char sceneName[256] = "main";
	char newObjectPath[256] = "data//objects//";

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
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unique_ptr<SpriteBatch> spriteBatch;
	std::unique_ptr<SpriteFont> spriteFont;


	VertexShader vertexShader;
	VertexShader vertexShaderMovable;

	PixelShader pixelShader;
	PixelShader pixelShader_noLight;
	PixelShader pixelShader_skybox;

	ConstantBuffer<CB_vertexShader> cb_vertexShader;
	ConstantBuffer<CB_pixelShader> cb_pixelShader;
};

