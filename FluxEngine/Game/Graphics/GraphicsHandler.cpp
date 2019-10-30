#include "GraphicsHandler.h"

bool GraphicsHandler::Init(HWND hWnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->hWnd = hWnd;
	fpsTimer.Start();

	if (!InitDirectX())
		return false;

	//if (!InitShaders())
	//	return false;

	//if (!InitScene())
	//	return false;

	//InitImGUI(hWnd);

	return true;
}

void GraphicsHandler::RenderFrame()
{
	static float bgColor[] = { 0.0f,0.0f,0.0f,1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (wireframe == false)
		this->deviceContext->RSSetState(this->rasterizerState.Get());
	else
		this->deviceContext->RSSetState(this->rasterizerState_FillWire.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); //this->blendState.Get()
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	//this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);

	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		SetWindowText(hWnd, fpsString.c_str());
		fpsCounter = 0;
		this->fpsTimer.Restart();
	}


	this->swapChain->Present(0, NULL);
}

GraphicsHandler::~GraphicsHandler()
{
}

bool GraphicsHandler::InitDirectX()
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No DXGI Adapters Found");
			return false;
		}

		int id = 0;
		if (adapters.size() > 1)
			for (size_t i = 0; i < adapters.size(); ++i)
				if (adapters[id].description.DedicatedVideoMemory < adapters[i].description.DedicatedVideoMemory)
					id = i;

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
			adapters[id].pAdapter,				///IDXGI Adapter (Largest VRAM)
			D3D_DRIVER_TYPE_UNKNOWN,			///D3D Driver
			nullptr,								///For software driver type
			0,								///Flags for runtime layers
			nullptr,								///Feature Levels Array
			0,									///Number of feature levels
			D3D11_SDK_VERSION,					///D3D SDK (Required)
			&scd,								///Swap Chain description
			this->swapChain.GetAddressOf(),		///&swapChain
			this->device.GetAddressOf(),		///&device
			nullptr,								///Supported feature level
			this->deviceContext.GetAddressOf()	///&deviceContext
		);
		EXCEPT_IF_FAILED(hr, "CreateDeviceAndSwapChain Failed");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		EXCEPT_IF_FAILED(hr, "GetBuffer Failed");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), nullptr, this->renderTargetView.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRenderTargetView Failed");

		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, nullptr, this->depthStencilBuffer.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateTexture2D for DepthStencilBuffer Failed");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), nullptr, this->depthStencilView.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateDepthStencilView for DepthStencilView Failed");

		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc(D3D11_DEFAULT);
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateDepthStencilState for DepthStencilState Failed");

		// Setup Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		this->deviceContext->RSSetViewports(1, &viewport);

		// Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");

		//RASTERIZER CULL FRONT
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");


		// Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc_FillWire(D3D11_DEFAULT);
		rasterizerDesc_FillWire.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_FillWire, this->rasterizerState_FillWire.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");

		// Create Blend State
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

		//// Setup Fonts
		//this->spriteBatch = std::make_unique<SpriteBatch>(this->deviceContext.Get());
		//this->spriteFont = std::make_unique<SpriteFont>(this->device.Get(), L"Data\\Fonts\\ComicSansMS_16.spritefont");

		// Create Sampler State
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
