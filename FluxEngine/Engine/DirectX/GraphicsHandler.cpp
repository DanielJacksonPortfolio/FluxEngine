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

RenderableGameObject* GraphicsHandler::SelectObject(float mouseX, float mouseY)
{
	float normalizedCoordinateX = (2.0f * mouseX) / windowWidth - 1.0f;
	float normalizedCoordinateY = 1.0f - (2.0f * mouseY) / windowHeight;
	normalizedCoordinateX = 0.0f; // Lock to Center
	normalizedCoordinateY = 0.0f;

	XMMATRIX inverseProjection = XMMatrixInverse(nullptr, projectionMatrix);
	XMMATRIX inverseView = XMMatrixInverse(nullptr, viewMatrix);

	XMVECTOR eyePos = this->camera->GetPositionVector();
	XMVECTOR rayOriginVector = XMVector3Transform(XMVector3Transform(XMVectorSet(normalizedCoordinateX, normalizedCoordinateY, 0, 0), inverseProjection), inverseView); // World Space
	XMVECTOR rayDirection = XMVector3Normalize(rayOriginVector - eyePos);

	PickRayDirection = rayDirection;
	PickRayOrigin = eyePos;

	std::vector<std::pair<RenderableGameObject*, float>> selectedObjects = {};

	for(int i = 0; i < objects.size(); ++i)
	{
		RenderableGameObject* object = objects[i].second;
		bool intersect = false;
		float nearestIntersect = INT_MAX;
		std::vector<Mesh*> meshes = object->GetMeshes();
		for (int i = 0; i < meshes.size(); ++i)
		{
			if (meshes[i]->RayMeshIntersect(eyePos, rayDirection, &nearestIntersect))
				intersect = true;
		}
		if (intersect)
			selectedObjects.push_back({ object, nearestIntersect });
	}

	if (selectedObjects.size() > 0)
	{
		RenderableGameObject* closestObject = nullptr;
		float closestObjectDistance = INT_MAX;
		for (size_t i = 0; i < selectedObjects.size(); ++i)
		{
			if (selectedObjects[i].second < closestObjectDistance)
			{
				closestObject = selectedObjects[i].first;
				closestObjectDistance = selectedObjects[i].second;
			}
		}
		return closestObject;
	}
	return nullptr;
}

void GraphicsHandler::RenderFrame()
{
	float bgColor[3] = {0.0f, 0.0f, 0.0f};
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgColor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (camera != nullptr)
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = static_cast<DWORD>(GetTickCount64());

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		this->cb_vertexShader.data.elapsedTime = (dwTimeCur - dwTimeStart) / 1000.0f;

		for(int i = 0; i < dLights.size(); ++i)
		{
			this->cb_pixelShader.data.directionalLightColor[i] = XMFLOAT4(dLights[i]->GetColor().x, dLights[i]->GetColor().y, dLights[i]->GetColor().z,1.0f);
			this->cb_pixelShader.data.directionalLightStrength[i] = XMFLOAT4(dLights[i]->GetStrength(),1.0f,1.0f,1.0f);
			this->cb_pixelShader.data.directionalLightDirection[i] = XMFLOAT4(dLights[i]->GetDirection().x, dLights[i]->GetDirection().y, dLights[i]->GetDirection().z,1.0f);
		}
		this->cb_pixelShader.data.numDLights = static_cast<int>(dLights.size());

		for (int i = 0; i < pLights.size(); ++i)
		{
			this->cb_pixelShader.data.pointLightColor[i] = XMFLOAT4(pLights[i]->GetColor().x, pLights[i]->GetColor().y, pLights[i]->GetColor().z, 1.0f);
			this->cb_pixelShader.data.pointLightFactors[i].x = pLights[i]->GetStrength();
			this->cb_pixelShader.data.pointLightPosition[i] = XMFLOAT4(pLights[i]->GetPositionFloat3().x, pLights[i]->GetPositionFloat3().y, pLights[i]->GetPositionFloat3().z,1.0f);
			this->cb_pixelShader.data.pointLightFactors[i].y = pLights[i]->GetCAttenuation();
			this->cb_pixelShader.data.pointLightFactors[i].z = pLights[i]->GetLAttenuation();
			this->cb_pixelShader.data.pointLightFactors[i].w = pLights[i]->GetEAttenuation();
		}
		this->cb_pixelShader.data.numPLights = static_cast<int>(pLights.size());

		this->cb_pixelShader.data.shininess = this->shininess;
		this->cb_pixelShader.data.cameraPosition = this->camera->GetPositionFloat3();
		this->cb_pixelShader.ApplyChanges();

		this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
		this->deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); //this->blendState.Get()
		this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

		///Shader Stuff///
		camera->SetProjectionValues(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 3000.0f);

		//Matrices
		this->viewMatrix = this->camera->GetViewMatrix();
		this->projectionMatrix = this->camera->GetProjectionMatrix();
		XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix;

		//Set Shader Layout
		this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());

		//Set Shaders

		if (camera != nullptr)
			skybox->SetPosition(camera->GetPositionFloat3());

		this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->pixelShader_skybox.GetShader(), NULL, 0);
		this->skybox->Draw(viewProjectionMatrix);
		this->deviceContext->PSSetShader(this->pixelShader.GetShader(), NULL, 0);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
		//while (it != objects.end())		//// SetUp Shader Bindable
		for(int i =0; i < objects.size(); ++i)
		{
			RenderableGameObject* object = objects[i].second;
			if (object != nullptr)
			{
				//Rasterizer States
				if (object->GetWireframeMode() == false)
					bindables["RSDefault"]->Bind();
				else
					bindables["RSNoneWireframe"]->Bind();
				this->cb_pixelShader.data.normalMap = object->GetNormalMapMode();
				this->cb_pixelShader.data.specularMap = object->GetSpecularMapMode();
				this->cb_pixelShader.data.grayscale = object->GetGrayscale();
				this->cb_pixelShader.ApplyChanges();
				if (object->GetName() == "Water")
				{
					this->deviceContext->VSSetShader(this->vertexShaderMovable.GetShader(), NULL, 0);
					object->Draw(viewProjectionMatrix);
					this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
				}
				else
					object->Draw(viewProjectionMatrix);
			}
			//it++;
		}

		if (showLights)
		{
			this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->pixelShader_noLight.GetShader(), NULL, 0);
			for (PointLight* light : pLights)
			{
				this->cb_pixelShader.data.pointLightColor[0] = XMFLOAT4(light->GetColor().x, light->GetColor().y, light->GetColor().z,1.0f);
				this->cb_pixelShader.ApplyChanges();
				light->Draw(viewProjectionMatrix);
			}
		}
	}
	///FPS Counter///

	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
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

void GraphicsHandler::SetObject(RenderableGameObject* obj)
{
	currentObject = obj;
}

void GraphicsHandler::SaveScene(std::string sceneName)
{
	std::ofstream objectsFile(sceneName + "_objects.txt");
	for(size_t i = 0; i < objects.size(); ++i)
	{
		std::string objectData = objects[i].second->Save();
		if (i+1 != objects.size())
			objectData += "\n";
		objectsFile << objectData;
	}
	objectsFile.close();

	std::ofstream camerasFile(sceneName + "_cameras.txt");
	for (int i = 0; i < cameras.size(); ++i)
	{
		std::string cameraData = cameras[i]->Save();
		if (i != cameras.size() - 1)
			cameraData += "\n";
		camerasFile << cameraData;
	}
	camerasFile.close();

	std::ofstream lightsFile(sceneName + "_lights.txt");
	for (int i = 0; i < pLights.size(); ++i)
	{
		std::string lightData = pLights[i]->Save();
		if (i != pLights.size() - 1 || dLights.size() != 0)
			lightData += "\n";
		lightsFile << lightData;
	}

	for (int i = 0; i < dLights.size(); ++i)
	{
		std::string lightData = dLights[i]->Save();
		if (i != dLights.size() - 1)
			lightData += "\n";
		lightsFile << lightData;
	}
	lightsFile.close();

	std::ofstream generalFile(sceneName + "_general.txt");
	std::string generalData = "";
	generalData += std::to_string(shininess) + ",";
	generalData += std::to_string(this->cb_pixelShader.data.ambientLightColor.x) + ",";
	generalData += std::to_string(this->cb_pixelShader.data.ambientLightColor.y) + ",";
	generalData += std::to_string(this->cb_pixelShader.data.ambientLightColor.z) + ",";
	generalData += std::to_string(this->cb_pixelShader.data.ambientLightStrength);
	generalFile << generalData;
	generalFile.close();
}

void GraphicsHandler::LoadScene(std::string sceneName)
{
	objectID = 0;
	cameraID = 0;
	pLightID = 0;
	dLightID = 0;

	DeleteObjects();
	std::ifstream objectsFile(sceneName + "_objects.txt");
	std::string line = "";
	//std::unordered_map<std::string, RenderableGameObject*> newObjects = {};
	std::vector<std::pair<std::string, RenderableGameObject*>> newObjects = {};
	while (std::getline(objectsFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		RenderableGameObject* newObject = new RenderableGameObject();
		if (newObject->Init(data, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader))
			newObjects.push_back(std::make_pair(data[0], newObject));
			//newObjects[data[0]] = newObject;
	}
	objectsFile.close();
	objects = newObjects;
	//if (objects.size() > 0)
	//	currentObject = objects[0].second; // NextObject();
	//else
	currentObject = nullptr;

	DeleteCameras();
	std::ifstream camerasFile(sceneName + "_cameras.txt");
	std::vector<Camera*> newCameras = {};
	while (std::getline(camerasFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		newCameras.push_back(new Camera(data));
	}
	camerasFile.close();
	cameras = newCameras;
	if (cameras.size() > 0)
		camera = cameras[0];//NextCamera();
	else
		camera = nullptr;
	
	std::ifstream generalFile(sceneName + "_general.txt");
	while (std::getline(generalFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		shininess = std::stof(data[0]);
		this->cb_pixelShader.data.ambientLightColor = XMFLOAT3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
		this->cb_pixelShader.data.ambientLightStrength = std::stof(data[4]);
	}
	generalFile.close();

	DeletePointLights();
	DeleteDirectionalLights();
	std::ifstream lightsFile(sceneName + "_lights.txt");
	std::vector<PointLight*> newPLights = {};
	std::vector<DirectionalLight*> newDLights = {};
	while (std::getline(lightsFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		if (data[0] == "P")
			newPLights.push_back(new PointLight(data, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader));
		else
			newDLights.push_back(new DirectionalLight(data, this->device.Get(), this->deviceContext.Get()));
	}
	lightsFile.close();

	pLights = newPLights;
	dLights = newDLights;
	if(pLights.size() > 0)
		NextPLight();
	else
		pointLight = nullptr;
	if(dLights.size() > 0)
		NextDLight();
	else
		directionalLight = nullptr;
}

void GraphicsHandler::RenderGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Debug Data");
	std::string originString = "3D Pick Ray Origin: " + std::to_string(XMVectorGetX(PickRayOrigin)) + ", " + std::to_string(XMVectorGetY(PickRayOrigin)) + ", " + std::to_string(XMVectorGetZ(PickRayOrigin));
	std::string dirString = "3D Pick Ray Direction: " + std::to_string(XMVectorGetX(PickRayDirection)) + ", " + std::to_string(XMVectorGetY(PickRayDirection)) + ", " + std::to_string(XMVectorGetZ(PickRayDirection));
	ImGui::Text(originString.c_str());
	ImGui::Text(dirString.c_str());
	ImGui::End();

	if (showUI)
	{
		if (showPLControls)
		{
			ImGui::Begin("Point Light Controls");
			if (pointLight != nullptr)
			{
				ImGui::ColorEdit3("Color", &this->pointLight->GetColor().x);
				ImGui::DragFloat("Strength", &this->pointLight->GetStrength(), 0.01f, 0.0f, 10.0f);
				XMFLOAT3 lightPos = this->pointLight->GetPositionFloat3();
				ImGui::DragFloat3("Position", &lightPos.x, 0.1f, -50.0f, 50.0f);
				this->pointLight->SetPosition(lightPos);

				ImGui::Text("Attenuation:");
				ImGui::DragFloat("Constant", &this->pointLight->GetCAttenuation(), 0.01f, 0.01f, 10.0f);
				ImGui::DragFloat("Linear", &this->pointLight->GetLAttenuation(), 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("Exponent", &this->pointLight->GetEAttenuation(), 0.01f, 0.0f, 10.0f);

				if (pLights.size() > 1)
				{
					if (ImGui::Button("Previous Light"))
						NextPLight(-1);
					ImGui::SameLine();
					if (ImGui::Button("Next Light"))
						NextPLight();
				}
			}
			if (ImGui::Button("New Point Light"))
				NewPointLight();
			if (pointLight != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Duplicate Light"))
					DuplicatePointLight();
				if (ImGui::Button("Delete Light"))
					DeletePointLight();
				ImGui::SameLine();
				if (ImGui::Button("Move To Camera"))
					pointLight->SetPosition(camera->GetPositionVector() + camera->GetForwardVector());
			}
			ImGui::End();
		}

		if (showDLControls)
		{
			ImGui::Begin("Directional Light Controls");
			if (directionalLight != nullptr)
			{
				ImGui::ColorEdit3("Color", &this->directionalLight->GetColor().x);
				ImGui::DragFloat("Strength", &this->directionalLight->GetStrength(), 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat3("Direction", &directionalLight->GetDirection().x, 0.01f, -1.0f, 1.0f);
				if (dLights.size() > 1)
				{
					if (ImGui::Button("Previous Light"))
						NextDLight(-1);
					ImGui::SameLine();
					if (ImGui::Button("Next Light"))
						NextDLight();
				}
			}
			if (ImGui::Button("New Directional Light"))
				NewDirectionalLight();
			if (directionalLight != nullptr)
			{
				if (ImGui::Button("Duplicate Light"))
					DuplicateDirectionalLight();
				if (ImGui::Button("Delete Light"))
					DeleteDirectionalLight();
				if (ImGui::Button("Set to Camera Direction"))
					directionalLight->SetPosition(camera->GetForwardVector() * -1);
			}
			ImGui::End();
		}
		if (showCamControls)
		{
			ImGui::Begin("Camera Controls");
			if (camera != nullptr)
			{
				ImGui::Text(camera->GetName().c_str());
				ImGui::DragFloat("Move Speed", &camera->GetVelocity().x, 0.005f, 0.0f, 5.0f);
				ImGui::DragFloat("FOV", &camera->GetFOV(), 0.1f, 0.01f, 179.99f);
				ImGui::Checkbox("Move Lock X?", &camera->GetLockedPos(0));
				if (!camera->GetLookAtMode())
				{
					ImGui::SameLine();
					ImGui::Checkbox("Pitch Lock?", &camera->GetLockedRot(0));
				}
				ImGui::Checkbox("Move Lock Y?", &camera->GetLockedPos(1));
				if (!camera->GetLookAtMode())
				{
					ImGui::SameLine();
					ImGui::Checkbox("Yaw Lock?", &camera->GetLockedRot(1));
				}
				ImGui::Checkbox("Move Lock Z?", &camera->GetLockedPos(2));
				if (!camera->GetLockedPos(0) || !camera->GetLockedPos(1) || !camera->GetLockedPos(2))
				{
					XMFLOAT3 camPos = this->camera->GetPositionFloat3();
					if (!camera->GetLockedPos(0))
					{
						if (!camera->GetLockedPos(1))
						{
							if (!camera->GetLockedPos(2))
								ImGui::DragFloat3("Position XYZ", &camPos.x, 0.1f, -80.0f, 80.0f);
							else
								ImGui::DragFloat2("Position XY", &camPos.x, 0.1f, -80.0f, 80.0f);
						}
						else
						{
							ImGui::DragFloat("Position X", &camPos.x, 0.1f, -80.0f, 80.0f);
							if (!camera->GetLockedPos(2))
								ImGui::DragFloat("Position Z", &camPos.z, 0.1f, -80.0f, 80.0f);
						}
					}
					else
					{
						if (!camera->GetLockedPos(1))
						{
							if (!camera->GetLockedPos(2))
								ImGui::DragFloat2("Position YZ", &camPos.y, 0.1f, -80.0f, 80.0f);
							else
								ImGui::DragFloat("Position Y", &camPos.y, 0.1f, -80.0f, 80.0f);
						}
						else
							ImGui::DragFloat("Position Z", &camPos.z, 0.1f, -80.0f, 80.0f);
					}
					this->camera->SetPosition(camPos);
				}
				ImGui::Checkbox("Look At Fixed Position?", &camera->GetLookAtMode());
				if (!camera->GetLookAtMode())
				{
					if (!camera->GetLockedRot(0) || !camera->GetLockedRot(1))
					{
						XMFLOAT3 camRot = this->camera->GetRotationFloat3();
						if (!camera->GetLockedRot(0))
							ImGui::DragFloat("Pitch", &camRot.x, 0.01f, -XM_PI, XM_PI);
						if (!camera->GetLockedRot(1))
							ImGui::DragFloat("Yaw", &camRot.y, 0.01f, -XM_PI, XM_PI);
						this->camera->SetRotation(camRot);
					}
				}
				else
				{
					ImGui::DragFloat3("Look At", &camera->GetLookAtPos().x, 0.1f, -50.0f, 50.0f);
					this->camera->SetLookAtPos();
				}
				if (cameras.size() > 1)
				{
					if (ImGui::Button("Previous Camera"))
						NextCamera(-1);
					ImGui::SameLine();
					if (ImGui::Button("Next Camera"))
						NextCamera();
				}
			}
			if (ImGui::Button("New Camera"))
				NewCamera();
			if (camera != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Duplicate Camera"))
					DuplicateCamera();
				if (ImGui::Button("Delete Camera"))
					DeleteCamera();
			}
			ImGui::End();
		}

		if (showObjectControls)
		{
			ImGui::Begin("Object Controls");
			if (currentObject != nullptr)
			{
				ImGui::Text(currentObject->GetName().c_str());
				ImGui::Checkbox("Draw?", &currentObject->GetRenderMode());
				if (currentObject->GetRenderMode())
				{
					ImGui::SameLine();
					ImGui::Checkbox("Movable?", &currentObject->GetMovable());
					if (currentObject->GetMovable())
						ImGui::DragFloat("Move Speed", &currentObject->GetVelocity().x, 0.005f, 0.0f, 5.0f);
					ImGui::Checkbox("Move Lock X?", &currentObject->GetLockedPos(0)); ImGui::SameLine();
					ImGui::Checkbox("Pitch Lock?", &currentObject->GetLockedRot(0));
					ImGui::Checkbox("Move Lock Y?", &currentObject->GetLockedPos(1)); ImGui::SameLine();
					ImGui::Checkbox("Yaw Lock?", &currentObject->GetLockedRot(1));
					ImGui::Checkbox("Move Lock Z?", &currentObject->GetLockedPos(2)); ImGui::SameLine();
					ImGui::Checkbox("Roll Lock?", &currentObject->GetLockedRot(2));

					if (!currentObject->GetLockedPos(0) || !currentObject->GetLockedPos(1) || !currentObject->GetLockedPos(2))
					{
						XMFLOAT3 objPos = this->currentObject->GetPositionFloat3();
						if (!currentObject->GetLockedPos(0))
						{
							if (!currentObject->GetLockedPos(1))
							{
								if (!currentObject->GetLockedPos(2))
									ImGui::DragFloat3("Position XYZ", &objPos.x, 0.1f, -80.0f, 80.0f);
								else
									ImGui::DragFloat2("Position XY", &objPos.x, 0.1f, -80.0f, 80.0f);
							}
							else
							{
								ImGui::DragFloat("Position X", &objPos.x, 0.1f, -80.0f, 80.0f);
								if (!currentObject->GetLockedPos(2))
									ImGui::DragFloat("Position Z", &objPos.z, 0.1f, -80.0f, 80.0f);
							}
						}
						else
						{
							if (!currentObject->GetLockedPos(1))
							{
								if (!currentObject->GetLockedPos(2))
									ImGui::DragFloat2("Position YZ", &objPos.y, 0.1f, -80.0f, 80.0f);
								else
									ImGui::DragFloat("Position Y", &objPos.y, 0.1f, -80.0f, 80.0f);
							}
							else
								ImGui::DragFloat("Position Z", &objPos.z, 0.1f, -80.0f, 80.0f);
						}
						this->currentObject->SetPosition(objPos);
					}
					if (!currentObject->GetLockedRot(0) || !currentObject->GetLockedRot(1) || !currentObject->GetLockedRot(2))
					{
						XMFLOAT3 objRot = this->currentObject->GetRotationFloat3();
						if (!currentObject->GetLockedRot(0))
							ImGui::DragFloat("Pitch", &objRot.x, 0.01f, -XM_PI, XM_PI);
						if (!currentObject->GetLockedRot(1))
							ImGui::DragFloat("Yaw", &objRot.y, 0.01f, -XM_PI, XM_PI);
						if (!currentObject->GetLockedRot(2))
							ImGui::DragFloat("Roll", &objRot.z, 0.01f, -XM_PI, XM_PI);
						this->currentObject->SetRotation(objRot);
					}
					ImGui::DragFloat("Scale", &currentObject->GetScale(), 0.01f, 0.01f, 10.0f);
					ImGui::SliderInt("Grayscale Mode", &currentObject->GetGrayscale(), 0, 2);
					ImGui::Checkbox("Wireframe?", &currentObject->GetWireframeMode()); ImGui::SameLine();
					ImGui::Checkbox("Use Normal Map?", &currentObject->GetNormalMapMode()); ImGui::SameLine();
					ImGui::Checkbox("Use Specular Map?", &currentObject->GetSpecularMapMode());
				}
				if (objects.size() > 1)
				{
					if (ImGui::Button("Previous Object"))
						NextObject(-1);
					ImGui::SameLine();
					if (ImGui::Button("Next Object"))
						NextObject();
				}

			}
			if (ImGui::Button("New Object"))
				showNewObjectWindow = true;
			if (currentObject != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Duplicate Object"))
					DuplicateObject();
				if (ImGui::Button("Delete Object"))
					DeleteObject();
				if (ImGui::Button("Lock Camera To This"))
				{
					camera->SetLookAtPos(currentObject->GetPositionFloat3());
					camera->SetLookAtMode(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Move To Camera"))
					currentObject->SetPosition(camera->GetPositionVector() + camera->GetForwardVector() * 10);
			}
			ImGui::End();
		}

		if (showNewObjectWindow)
		{
			ImGui::Begin("Create a New Object");
			ImGui::InputText("Object Filepath", &newObjectPath[0], ARRAYSIZE(newObjectPath));
			if (ImGui::Button("New Object"))
				NewObject();
			ImGui::End();
		}

		if (showGeneralControls)
		{
			ImGui::Begin("General Controls");
			ImGui::Checkbox("VSync Enabled", &config->vSync); ImGui::SameLine();
			ImGui::Checkbox("Show Lights?", &showLights);
			ImGui::DragFloat("Universal Shininess", &this->shininess, 1.0f, 1.0f, 64.0f);
			ImGui::ColorEdit3("Ambient Color", &this->cb_pixelShader.data.ambientLightColor.x);
			ImGui::DragFloat("Ambient Strength", &this->cb_pixelShader.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
			ImGui::InputText("Scene Name", &sceneName[0], ARRAYSIZE(sceneName));
			if (ImGui::Button("Save Scene"))
				SaveScene("data//scenes//" + std::string(sceneName));
			ImGui::SameLine();
			if (ImGui::Button("Load Scene"))
				LoadScene("data//scenes//" + std::string(sceneName));
			ImGui::End();
		}


		ImGui::Begin("Show/Hide Windows");
		ImGui::Checkbox("Show Error Log?", &ErrorLogger::showErrorLog);
		ImGui::Checkbox("Show Point Light Controls?", &showPLControls);
		ImGui::Checkbox("Show Directional Light Controls?", &showDLControls);
		ImGui::Checkbox("Show Camera Controls?", &showCamControls);
		ImGui::Checkbox("Show Object Controls?", &showObjectControls);
		ImGui::Checkbox("Show General Controls?", &showGeneralControls);
		ImGui::End();

		if (ErrorLogger::showErrorLog)
		{
			ImGui::Begin("Error Log");
			ImGui::Text(ErrorLogger::ERROR_LOG.c_str());
			ImGui::AlignTextToFramePadding();
			ImGui::End();
		}
	}
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsHandler::NextCamera(int direction)
{
	cameraID = (cameraID + (direction >= 0 ? 1 : -1)) % static_cast<int>(cameras.size());
	if (cameraID < 0)
		cameraID += static_cast<int>(cameras.size());
	camera = cameras[cameraID];
}

void GraphicsHandler::NextPLight(int direction)
{
	pLightID = (pLightID + (direction >= 0 ? 1 : -1)) % static_cast<int>(pLights.size());
	if (pLightID < 0)
		pLightID += static_cast<int>(pLights.size());
	pointLight = pLights[pLightID];
}
void GraphicsHandler::NextDLight(int direction)
{
	dLightID = (dLightID + (direction >= 0 ? 1 : -1)) % static_cast<int>(dLights.size());
	if (dLightID < 0)
		dLightID += static_cast<int>(dLights.size());
	directionalLight = dLights[dLightID];
}
void GraphicsHandler::NextObject(int direction)
{
	objectID = (objectID + (direction >= 0 ? 1 : -1)) % static_cast<int>(objects.size());
	if (objectID < 0)
		objectID += static_cast<int>(objects.size());
	//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
	//for(int i = 0; i < objectID; ++i)
	//	it++;
	currentObject = objects[objectID].second;//it->second;
}

void GraphicsHandler::NewCamera()
{
	Camera* camera = new Camera();
	cameras.insert(cameras.begin() + cameraID, camera);
	this->camera = cameras[cameraID];
}

void GraphicsHandler::DuplicateCamera()
{
	Camera* camera = new Camera(*this->camera);
	camera->GetName() += " - Copy";
	cameras.insert(cameras.begin() + cameraID+1, camera);
	cameraID++;
	this->camera = camera;
}

void GraphicsHandler::DeleteCamera()
{
	delete camera;
	camera = nullptr;
	cameras.erase(cameras.begin() + (cameraID < 0 ? 0 : cameraID--));
	if (cameras.size() > 0)
		NextCamera();
	else
		cameraID = 0;
}

void GraphicsHandler::DeleteCameras()
{
	for (int i = 0; i < cameras.size(); ++i)
		delete cameras[i];
	cameras.clear();
}
void GraphicsHandler::DeleteObjects()
{
	//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
	//while (it != objects.end())
	for(int i = 0; i < objects.size(); ++i)
	{
		delete objects[i].second;
		//delete it->second;
		//it++;
	}
	objects.clear();
}
void GraphicsHandler::DeletePointLights()
{
	for (int i = 0; i < pLights.size(); ++i)
		delete pLights[i];
	pLights.clear();
}
void GraphicsHandler::DeleteDirectionalLights()
{
	for (int i = 0; i < dLights.size(); ++i)
		delete dLights[i];
	dLights.clear();
}

void GraphicsHandler::DeleteObject()
{
	//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
	//while (it->first != currentObject->GetName())
	//	it++;
	int deleteObjectID = 0;
	for (deleteObjectID; deleteObjectID < objects.size(); ++deleteObjectID)
		if (objects[deleteObjectID].first == currentObject->GetName())
			break;
	delete currentObject;
	currentObject = nullptr;
	objects.erase(objects.begin() + deleteObjectID);
	if (objects.size() > 0)
		NextObject();
	else
		objectID = 0;
}

void GraphicsHandler::NewObject()
{
	showNewObjectWindow = false;
	RenderableGameObject* newObject = new RenderableGameObject();
	std::string directory = StringTools::GetDirectoryFromPath(newObjectPath);
	std::string fileExtension = StringTools::GetFileExtension(newObjectPath);
	try
	{
		std::string name = std::string(newObjectPath).substr(directory.length() + 1, std::string(newObjectPath).length() - directory.length() - fileExtension.length() - 2);
		if (newObject->Init(newObjectPath, 1.0f, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader, name))
		{
			//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
			//for (int i = 0; i < objectID; ++i)
			//	it++;
			objects.insert(objects.begin() + objectID, { newObject->GetName(), newObject });
			objectID--;
			this->currentObject = newObject;
		}
	}
	catch(std::exception& e)
	{
		ErrorLogger::Log(e, "No filepath given when creating New Object");
	}
	memset(newObjectPath, 0, sizeof(newObjectPath));
}

void GraphicsHandler::DuplicateObject()
{
	RenderableGameObject* object = new RenderableGameObject(*currentObject);
	object->GetName() += " - Copy";
	//std::unordered_map<std::string, RenderableGameObject*>::iterator it = objects.begin();
	//for (int i = 0; i < objectID; ++i)
	//	it++;
	objects.insert(objects.begin() + objectID, { object->GetName(), object });
	objectID--;
	this->currentObject = object;
}
void GraphicsHandler::NewPointLight()
{
	PointLight* pLight = new PointLight();
	pLight->Init(this->device.Get(), this->deviceContext.Get(), this->cb_vertexShader, this->cb_pixelShader);
	pLights.insert(pLights.begin() + pLightID, pLight);
	this->pointLight = pLights[pLightID];
}

void GraphicsHandler::DuplicatePointLight()
{
	PointLight* light = new PointLight(*pointLight);
	pLights.insert(pLights.begin()+pLightID+1, light);
	pLightID++;
	this->pointLight = light;
}

void GraphicsHandler::DeletePointLight()
{
	delete pointLight;
	pointLight = nullptr;
	pLights.erase(pLights.begin() + (pLightID < 0 ? 0 : pLightID--));
	if (pLights.size() > 0)
		NextPLight();
	else
		pLightID = 0;
}
void GraphicsHandler::NewDirectionalLight()
{
	DirectionalLight* dLight = new DirectionalLight();
	dLight->Init(this->device.Get(), this->deviceContext.Get());
	dLights.insert(dLights.begin() + dLightID, dLight);
	this->directionalLight = dLights[dLightID];
}

void GraphicsHandler::DuplicateDirectionalLight()
{
	DirectionalLight* light = new DirectionalLight(*directionalLight);
	dLights.insert(dLights.begin() + dLightID+1, light);
	dLightID++;
	this->directionalLight = light;
}

void GraphicsHandler::DeleteDirectionalLight()
{
	delete directionalLight;
	directionalLight = nullptr;
	dLights.erase(dLights.begin() + (dLightID < 0 ? 0 : dLightID--));
	if (dLights.size() > 0)
		NextDLight();
	else
		dLightID = 0;
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

		bindables["RSDefault"] = new RasterizerState(this->device, this->deviceContext);
		bindables["RSWireframe"] = new RasterizerState(this->device, this->deviceContext, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);
		bindables["RSFront"] = new RasterizerState(this->device, this->deviceContext, D3D11_CULL_FRONT);
		bindables["RSFrontWireframe"] = new RasterizerState(this->device, this->deviceContext, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME);
		bindables["RSNone"] = new RasterizerState(this->device, this->deviceContext, D3D11_CULL_NONE);
		bindables["RSNoneWireframe"] = new RasterizerState(this->device, this->deviceContext, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);

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
		{"NORMAL",		0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT",		0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL",	0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	//Vertex Shaders
	vertexShader.Init(this->device, shaderFolder + L"vertexShader_ADS_Specular.cso", layout, numElements);
	vertexShaderMovable.Init(this->device, shaderFolder + L"vertexShader_ADS_Specular_Move.cso", layout, numElements);

	//Pixel Shaders
	pixelShader.Init(this->device, shaderFolder + L"pixelShader_ADS_Specular.cso");
	pixelShader_noLight.Init(this->device, shaderFolder + L"pixelShader_NoLight.cso");
	pixelShader_skybox.Init(this->device, shaderFolder + L"pixelShader_Skybox.cso");

	return true;
}

bool GraphicsHandler::InitScene()
{
	try
	{
		//Constant Buffers
		HRESULT hr = cb_vertexShader.Init(this->device.Get(), this->deviceContext.Get());
		EXCEPT_IF_FAILED(hr, "Failed to initialize cb_vertexShader"); 
		
		hr = cb_pixelShader.Init(this->device.Get(), this->deviceContext.Get());
		EXCEPT_IF_FAILED(hr, "Failed to initialize cb_pixelShader");

		//Model Load
		LoadScene("data//scenes//initial");
		skybox = new RenderableGameObject();
		skybox->Init("data//skies//desert.fbx",10.0f, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader,"SKYBOX");
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}