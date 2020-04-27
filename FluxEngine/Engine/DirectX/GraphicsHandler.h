#pragma once

#include "../Game Objects/Scene.h"

#include "AdapterHandler.h"
#include "../Buffers and Shaders/VertexShader.h"
#include "../Buffers and Shaders/PixelShader.h"
#include "../../Tools/Timer.h"
#include "../../Resources/Config.h"

#include "../Game Objects/Camera.h"
#include "../Game Objects/Lights/PointLight.h"
#include "../Game Objects/Lights/DirectionalLight.h"
#include "../Physics/Collisions/CollisionHandler.h"

#include "../Bindables/RasterizerState.h"

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_win32.h>
#include <ImGUI/imgui_impl_dx11.h>

#include <memory>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <unordered_map>

#include "../Physics/Forces/ForceRegistry.h"
#include "../Physics/Forces/Gravity.h"
#include "../Physics/Forces/Drag.h"
#include "../Physics/Forces/Spring.h"
#include "../Physics/Forces/AnchoredSpring.h"
#include "../Physics/Forces/Bungee.h"
#include "../Physics/Forces/AnchoredBungee.h"
#include "../Physics/Forces/Buoyancy.h"

class GraphicsHandler
{
public:

	bool Init(HWND hWnd, int width, int height, Config* config);
	void RenderFrame();
	void SetObject(PropObject* obj);
	void PushObject(PropObject* obj, XMVECTOR rayDirection, XMVECTOR collisionLocation);
	PropObject* PickObject(float x, float y, XMVECTOR& rayDirection, XMVECTOR& collisionLocation);
	void Update(float dt);

	Camera* camera;
	PropObject* currentObject;
	PointLight* pointLight;
	DirectionalLight* directionalLight;

	bool showUI = false;
	void ToggleGravity();

private:
	bool InitDirectX();
	bool InitShaders();
	bool InitScene();
	void InitImGUI();

	void RenderGUI();
	void ResolveCollisions();

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
	std::vector<std::pair<std::string, PropObject*>> objects = {};

	int cameraID = 0;
	int pLightID = 0;
	int dLightID = 0;
	int objectID = 0;

	PropObject* skybox;
	PropObject* floor;
	Model* debugSphere;
	Model* debugCube;
	std::unordered_map<std::string, Bindable*> bindables = {};

	bool showLights = true;
	bool showPLControls = true;
	bool showDLControls = true;
	bool showCamControls = true;
	bool showObjectControls = true;
	bool showGeneralControls = true;
	bool showNewObjectWindow = false;
	bool showDebugWindow = false;
	bool debugMode = false;

	bool gravityEnabled = true;
	float shininess = 8.0f;
	char sceneName[256] = "initial";
	char newObjectPath[256] = "data//objects//";
	float floorHeight = 0.0f;
	float pushStrength = 20.0f;

	XMVECTOR pickRayOrigin = XMVECTOR();
	XMVECTOR pickRayDirection = XMVECTOR();
	std::string objectSelectedID = "No Object Selected";
	int numObjectsSelected = 0;

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

	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMMatrixIdentity();

	ConstantBuffer<CB_vertexShader> cb_vertexShader;
	ConstantBuffer<CB_pixelShader> cb_pixelShader;

	Scene* scene = nullptr;

	Gravity* gravity = nullptr;
	Drag* drag = nullptr;
	AnchoredSpring* springA = nullptr;

	ForceRegistry forceRegistry = ForceRegistry();
};

