#include "GraphicsHandler.h"


bool GraphicsHandler::Init(HWND hWnd, int width, int height, Config* config)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->hWnd = hWnd;
	this->config = std::make_unique<Config>(*config);
	fpsTimer.Start();

	if (!InitDirectX())
		return false;

	if (!InitShaders())
		return false;

	if (!InitScene())
		return false;

	InitImGUI();

	return true;
}

void GraphicsHandler::RenderFrame()
{

	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Rasterizer State
	if (wireframe == false)
		this->deviceContext->RSSetState(this->rasterizerState.Get());
	else
		this->deviceContext->RSSetState(this->rasterizerState_WireFrame.Get());

	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); //this->blendState.Get()
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	///Shader Stuff///

	//Matrices
	XMMATRIX worldMatrix = XMMatrixIdentity(); //XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XM_PIDIV2);
	XMMATRIX viewProjectionMatrix = this->camera.GetViewMatrix() * this->camera.GetProjectionMatrix();

	//Setup Constant Buffer - cb_vertexShader_PosTex
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vertexShader.GetAddressOf());
	this->cb_vertexShader.data.wvpMatrix = worldMatrix * viewProjectionMatrix;
	this->cb_vertexShader.data.worldMatrix = worldMatrix;
	this->cb_vertexShader.ApplyChanges();

	//Set Shader Layout
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());

	//Set Shaders
	this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelShader.GetShader(), NULL, 0);

	cube.Draw(viewProjectionMatrix);

	///FPS Counter///

	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		//SetWindowText(hWnd, fpsString.c_str());
		fpsCounter = 0;
		this->fpsTimer.Restart();
	}

	//Render Font
	this->spriteBatch->Begin();
	this->spriteFont->DrawString(this->spriteBatch.get(), StringTools::StandardToWide(fpsString).c_str(), XMFLOAT2(10, 10), DirectX::Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	this->spriteBatch->End();

	//GUI
	RenderGUI();

	this->swapChain->Present(config->vSync, NULL);
}

void GraphicsHandler::RenderGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Lighting");
	ImGui::DragFloat3("Background Color", bgColor, 0.01f, 0.0f, 1.0f);
	ImGui::End();
	
	ImGui::Begin("Debug");
	ImGui::DragFloat("Camera Speed", &cameraSpeed, 0.005f, 0.0f, 5.0f);
	ImGui::Checkbox("Wireframe?", &wireframe);
	ImGui::End();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool GraphicsHandler::InitDirectX()
{
	try
	{
		//Adapters
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No DXGI Adapters Found");
			return false;
		}

		//Get Largest VRAM adapterID
		int id = 0;
		if (adapters.size() > 1)
			for (int i = 0; i < static_cast<int>(adapters.size()); ++i)
				if (adapters[id].description.DedicatedVideoMemory < adapters[i].description.DedicatedVideoMemory)
					id = i;

		//Swap Chain
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = this->hWnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			adapters[id].pAdapter,				//IDXGI Adapter (Largest VRAM)
			D3D_DRIVER_TYPE_UNKNOWN,			//D3D Driver
			nullptr,							//For software driver type
			D3D11_CREATE_DEVICE_DEBUG,			//Flags for runtime layers
			nullptr,							//Feature Levels Array
			0,									//Number of feature levels
			D3D11_SDK_VERSION,					//D3D SDK (Required)
			&scd,								//Swap Chain description
			this->swapChain.GetAddressOf(),		//&swapChain
			this->device.GetAddressOf(),		//&device
			nullptr,							//Supported feature level
			this->deviceContext.GetAddressOf()	//&deviceContext
		);
		EXCEPT_IF_FAILED(hr, "CreateDeviceAndSwapChain Failed");

		//Back Buffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		EXCEPT_IF_FAILED(hr, "GetBuffer Failed");

		//Render Target View
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), nullptr, this->renderTargetView.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRenderTargetView Failed");

		//Depth Stencil
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, nullptr, this->depthStencilBuffer.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateTexture2D for DepthStencilBuffer Failed");

		//Depth Stencil View
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), nullptr, this->depthStencilView.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateDepthStencilView for DepthStencilView Failed");

		//OMSet Render Targets
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Depth Stencil State
		CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc(D3D11_DEFAULT);
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateDepthStencilState for DepthStencilState Failed");

		// Setup Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		this->deviceContext->RSSetViewports(1, &viewport);

		//Rasterizer State - Default
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");

		//Rasterizer State - Cull Front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");

		//Rasterizer State - Wireframe
		CD3D11_RASTERIZER_DESC rasterizerDesc_WireFrame(D3D11_DEFAULT);
		rasterizerDesc_WireFrame.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_WireFrame, this->rasterizerState_WireFrame.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");
		
		//Rasterizer State - Cull Front Wireframe
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront_WireFrame(D3D11_DEFAULT);
		rasterizerDesc_CullFront_WireFrame.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		rasterizerDesc_CullFront_WireFrame.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront_WireFrame, this->rasterizerState_CullFront_WireFrame.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");

		//Blend State
		D3D11_BLEND_DESC blendDesc = { 0 };

		D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = { 0 };
		renderTargetBlendDesc.BlendEnable = true;
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = renderTargetBlendDesc;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateBlendState Failed");

		//Setup Fonts
		this->spriteBatch = std::make_unique<SpriteBatch>(this->deviceContext.Get());
		this->spriteFont = std::make_unique<SpriteFont>(this->device.Get(), L"data\\fonts\\ComicSansMS_16.spritefont");

		//Sampler State
		CD3D11_SAMPLER_DESC samplerStateDesc(D3D11_DEFAULT);
		samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&samplerStateDesc, this->samplerState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateSamplerState Failed");
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

void GraphicsHandler::InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(this->hWnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();
}

bool GraphicsHandler::InitShaders()
{
	std::wstring shaderFolder = L"";
	#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
		#ifdef _DEBUG
			#ifdef _WIN64
				shaderFolder = L"..\\x64\\Debug\\";
			#else
				shaderFolder = L"..\\Debug\\";
			#endif
		#else
			#ifdef _WIN64
				shaderFolder = L"..\\x64\\Release\\";
			#else
				shaderFolder = L"..\\Release\\";
			#endif
		#endif
	}


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",	0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,								D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD",	0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	UINT numElements = ARRAYSIZE(layout);

	//Vertex Shaders
	if (!vertexShader.Init(this->device, shaderFolder + L"vertexShader_PosTex.cso", layout, numElements))
		return false;

	//Pixel Shaders
	if (!pixelShader.Init(this->device, shaderFolder + L"pixelShader_PosTex.cso"))
		return false;

	return true;
}

bool GraphicsHandler::InitScene()
{
	try
	{
		//Constant Buffers
		HRESULT hr = cb_vertexShader.Init(this->device.Get(), this->deviceContext.Get());
		EXCEPT_IF_FAILED(hr, "Failed to initialize cb_vertexShader_PosCol_3D");

		hr = cb_vertexShader.Init(this->device.Get(), this->deviceContext.Get());
		EXCEPT_IF_FAILED(hr, "Failed to initialize cb_vertexShader_PosTex");

		//Model Load
		cube.Init("", this->device.Get(), this->deviceContext.Get(), cb_vertexShader);

		//Camera(s)
		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 3000.0f);
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}