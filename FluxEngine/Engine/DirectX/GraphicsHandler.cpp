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

PropObject* GraphicsHandler::PickObject(float mouseX, float mouseY, XMVECTOR& rayDirection, XMVECTOR& collisionLocation)
{
	float normalizedCoordinateX = (2.0f * mouseX) / windowWidth - 1.0f;
	float normalizedCoordinateY = 1.0f - (2.0f * mouseY) / windowHeight;
	normalizedCoordinateX = 0.0f; // Lock to Center
	normalizedCoordinateY = 0.0f;
	
	XMVECTOR determinant = XMVECTOR();
	XMMATRIX inverseProjection = XMMatrixInverse(&determinant, projectionMatrix);
	XMMATRIX inverseView = XMMatrixInverse(&determinant, viewMatrix);

	XMVECTOR eyePos = this->camera->GetTransform()->GetPosition();
	XMVECTOR rayOriginVector = XMVector3Transform(XMVector3Transform(XMVectorSet(normalizedCoordinateX, normalizedCoordinateY, -1, 0), inverseProjection), inverseView); // World Space
	rayDirection = XMVector3Normalize(rayOriginVector - eyePos);

	pickRayDirection = rayDirection;
	pickRayOrigin = eyePos;

	struct Object
	{
		PropObject* object;
		float intersectDistance;
		XMVECTOR intersectLocation;
		Object(PropObject* object, float intersectDistance, XMVECTOR intersectLocation) : object(object), intersectDistance(intersectDistance), intersectLocation(intersectLocation) {}
	};

	std::vector<Object> selectedObjects = {};

	for(int i = 0; i < objects.size(); ++i)
	{
		PropObject* object = objects[i].second;
		float nearestIntersect = INT_MAX;
		XMVECTOR intersectLocation = XMVECTOR();
		if (CollisionHandler::Instance()->RayPropIntersect(object, eyePos, rayDirection, nearestIntersect, intersectLocation))
			selectedObjects.push_back(Object(object, nearestIntersect,intersectLocation));
	}

	numObjectsSelected = static_cast<int>(selectedObjects.size());

	if (selectedObjects.size() > 0)
	{
		PropObject* closestObject = selectedObjects[0].object;
		float closestObjectDistance = selectedObjects[0].intersectDistance;
		int closestObjectID = 0;

		for (size_t i = 1; i < selectedObjects.size(); ++i)
			if (selectedObjects[i].intersectDistance < closestObjectDistance)
			{
				closestObjectID = static_cast<int>(i);
				closestObjectDistance = selectedObjects[closestObjectID].intersectDistance;
			}

		closestObject = selectedObjects[closestObjectID].object;
		collisionLocation = selectedObjects[closestObjectID].intersectLocation;

		objectSelectedID = "Object Selected: " + closestObject->GetName();
		return closestObject;
	}
	objectSelectedID = "No Object Selected";
	return nullptr;
}

void GraphicsHandler::Update(float dt)
{
	ResolvePenetrations();

	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (gravityEnabled)
		{
			objects[i].second->GetPhysics()->AddLinearForce(GRAVITY * objects[i].second->GetPhysics()->GetMass()); //Gravity
			if (XMVectorGetY(objects[i].second->GetTransform()->GetPosition() + objects[i].second->GetAppearance()->GetModel()->minAABBCoord * objects[i].second->GetTransform()->GetScale()) < floorHeight)
			{
				objects[i].second->GetPhysics()->AddLinearForce(-GRAVITY * objects[i].second->GetPhysics()->GetMass());
				objects[i].second->GetTransform()->AdjustPosition(0, floorHeight-XMVectorGetY(objects[i].second->GetTransform()->GetPosition() + objects[i].second->GetAppearance()->GetModel()->minAABBCoord * objects[i].second->GetTransform()->GetScale()), 0);
				objects[i].second->GetPhysics()->SetLVelocity(CollisionHandler::Instance()->VectorReflection(objects[i].second->GetPhysics()->GetLVelocity(),XMVectorSet(0.0f,1.0f,0.0f,1.0f)) * 0.5);
			}
		}
			
		objects[i].second->Update(dt);
	}

	for (size_t i = 0; i < cameras.size(); ++i)
	{
		cameras[i]->Update(dt);
	}
	for (size_t i = 0; i < pLights.size(); ++i)
	{
		pLights[i]->Update(dt);
	}
	for (size_t i = 0; i < dLights.size(); ++i)
	{
		dLights[i]->Update(dt);
	}

	floor->GetTransform()->SetPosition(XMVectorGetX(floor->GetTransform()->GetPosition()), floorHeight, XMVectorGetZ(floor->GetTransform()->GetPosition()));
	floor->Update(dt);

	skybox->GetTransform()->SetPosition(camera->GetTransform()->GetPosition());
	this->skybox->Update(dt);
}


void GraphicsHandler::SetObject(PropObject* obj)
{
	currentObject = obj;
}

void GraphicsHandler::PushObject(PropObject* obj, XMVECTOR rayDirection, XMVECTOR collisionLocation)
{
	if (obj != nullptr)
	{
		obj->GetPhysics()->AddLinearForce(rayDirection * pushStrength*obj->GetPhysics()->GetMass());
		obj->GetPhysics()->AddTorque(rayDirection * pushStrength, collisionLocation);
	}
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
		this->cb_vertexShader.ApplyChanges();

		for(int i = 0; i < dLights.size(); ++i)
		{
			this->cb_pixelShader.data.directionalLightColor[i] = XMFLOAT4(dLights[i]->GetColor().x, dLights[i]->GetColor().y, dLights[i]->GetColor().z,1.0f);
			this->cb_pixelShader.data.directionalLightStrength[i] = XMFLOAT4(dLights[i]->GetStrength(),1.0f,1.0f,1.0f);
			this->cb_pixelShader.data.directionalLightDirection[i] = XMFLOAT4(XMVectorGetX(dLights[i]->GetDirection()), XMVectorGetY(dLights[i]->GetDirection()), XMVectorGetZ(dLights[i]->GetDirection()),1.0f);
		}
		this->cb_pixelShader.data.numDLights = static_cast<int>(dLights.size());

		for (int i = 0; i < pLights.size(); ++i)
		{
			this->cb_pixelShader.data.pointLightColor[i] = XMFLOAT4(pLights[i]->GetColor().x, pLights[i]->GetColor().y, pLights[i]->GetColor().z, 1.0f);
			this->cb_pixelShader.data.pointLightFactors[i].x = pLights[i]->GetStrength();
			this->cb_pixelShader.data.pointLightPosition[i] = XMFLOAT4(XMVectorGetX(pLights[i]->GetTransform()->GetPosition()), XMVectorGetY(pLights[i]->GetTransform()->GetPosition()), XMVectorGetZ(pLights[i]->GetTransform()->GetPosition()),1.0f);
			this->cb_pixelShader.data.pointLightFactors[i].y = pLights[i]->GetCAttenuation();
			this->cb_pixelShader.data.pointLightFactors[i].z = pLights[i]->GetLAttenuation();
			this->cb_pixelShader.data.pointLightFactors[i].w = pLights[i]->GetEAttenuation();
		}
		this->cb_pixelShader.data.numPLights = static_cast<int>(pLights.size());

		this->cb_pixelShader.data.shininess = this->shininess;
		XMStoreFloat3(&this->cb_pixelShader.data.cameraPosition, this->camera->GetTransform()->GetPosition());
		this->cb_pixelShader.ApplyChanges();

		this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
		this->deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); //this->blendState.Get()
		this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

		//Matrices
		this->viewMatrix = this->camera->GetViewMatrix();
		this->projectionMatrix = this->camera->GetProjectionMatrix();
		XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix;

		//Set Shader Layout
		this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());

		//Set Shaders

		this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->pixelShader_skybox.GetShader(), NULL, 0);
		this->skybox->Draw(viewProjectionMatrix);
		this->deviceContext->PSSetShader(this->pixelShader.GetShader(), NULL, 0);
		this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


		//// SetUp Shader Bindable
		for(int i = 0; i < objects.size(); ++i)
		{
			PropObject* object = objects[i].second;
			if (object != nullptr)
			{
				//Rasterizer States
				if (object->GetAppearance()->GetWireframeMode() == false)
					bindables["RSDefault"]->Bind();
				else
					bindables["RSNoneWireframe"]->Bind();
				this->cb_pixelShader.data.normalMap = object->GetAppearance()->GetNormalMapMode();
				this->cb_pixelShader.data.specularMap = object->GetAppearance()->GetSpecularMapMode();
				this->cb_pixelShader.data.grayscale = object->GetAppearance()->GetGrayscale();
				this->cb_pixelShader.ApplyChanges();
				if (object->GetName() == "Water")
				{
					bindables["RSNone"]->Bind();
					this->deviceContext->VSSetShader(this->vertexShaderMovable.GetShader(), NULL, 0);
					object->Draw(viewProjectionMatrix);
					this->deviceContext->VSSetShader(this->vertexShader.GetShader(), NULL, 0);
				}
				else
					object->Draw(viewProjectionMatrix);
				if(debugMode)
					object->DrawDebug(viewProjectionMatrix, debugSphere, debugCube);
			}
		}

		this->floor->Draw(viewProjectionMatrix);

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
	this->spriteFont->DrawString(this->spriteBatch.get(), StringTools::StandardToWide("X").c_str(), XMFLOAT2(windowWidth*0.5f - 10, windowHeight * 0.5f - 16), DirectX::Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	this->spriteBatch->End();

	//GUI
	RenderGUI();

	this->swapChain->Present(config->vSync, NULL);
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
	generalData += std::to_string(this->cb_pixelShader.data.ambientLightStrength) + ",";
	generalData += std::to_string(floorHeight);
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
	std::vector<std::pair<std::string, PropObject*>> newObjects = {};
	while (std::getline(objectsFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		PropObject* newObject = new PropObject();
		if (newObject->Init(data, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader))
			newObjects.push_back(std::make_pair(data[0], newObject));
	}
	objectsFile.close();
	objects = newObjects;
	if (objects.size() > 0)
		currentObject = objects[0].second; // NextObject();
	else
		currentObject = nullptr;

	DeleteCameras();
	std::ifstream camerasFile(sceneName + "_cameras.txt");
	std::vector<Camera*> newCameras = {};
	while (std::getline(camerasFile, line))
	{
		std::vector<std::string> data = {};
		StringTools::SplitString(line, data, ',');
		Camera* cam = new Camera();
		cam->Init(data);
		newCameras.push_back(cam);
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
		this->floorHeight = std::stof(data[5]);
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

	if (showDebugWindow)
	{
		ImGui::Begin("Debug Data");
		std::string originString = "3D Pick Ray Origin: " + std::to_string(XMVectorGetX(pickRayOrigin)) + ", " + std::to_string(XMVectorGetY(pickRayOrigin)) + ", " + std::to_string(XMVectorGetZ(pickRayOrigin));
		std::string dirString = "3D Pick Ray Direction: " + std::to_string(XMVectorGetX(pickRayDirection)) + ", " + std::to_string(XMVectorGetY(pickRayDirection)) + ", " + std::to_string(XMVectorGetZ(pickRayDirection));
		std::string numSelectedString = "Number of Objects Selected: " + std::to_string(numObjectsSelected);
		ImGui::Checkbox("Debug Mode", &debugMode);
		ImGui::Text(originString.c_str());
		ImGui::Text(dirString.c_str());
		ImGui::Text(objectSelectedID.c_str());
		ImGui::Text(numSelectedString.c_str());
		ImGui::End();
	}

	if (showUI)
	{
		if (showPLControls)
		{
			ImGui::Begin("Point Light Controls");
			if (pointLight != nullptr)
			{
				ImGui::ColorEdit3("Color", &this->pointLight->GetColor().x);
				ImGui::DragFloat("Strength", &this->pointLight->GetStrength(), 0.01f, 0.0f, 10.0f);
				XMFLOAT3 lightPos;
				XMStoreFloat3(&lightPos, this->pointLight->GetTransform()->GetPosition());
				ImGui::DragFloat3("Position", &lightPos.x, 0.1f, -50.0f, 50.0f);
				this->pointLight->GetTransform()->SetPosition(lightPos);
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
				{
					pointLight->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + camera->GetTransform()->GetForwardVector());
				}
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
				XMFLOAT3 lightDir;
				XMStoreFloat3(&lightDir, this->directionalLight->GetDirection());
				ImGui::DragFloat3("Direction", &lightDir.x, 0.01f, -1.0f, 1.0f);
				this->directionalLight->GetTransform()->SetPosition(lightDir);

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
				{
					directionalLight->GetTransform()->SetPosition(camera->GetTransform()->GetForwardVector() * -1);
				}
			}
			ImGui::End();
		}
		if (showCamControls)
		{
			ImGui::Begin("Camera Controls");
			if (camera != nullptr)
			{
				ImGui::Text(camera->GetName().c_str());
				ImGui::DragFloat("Move Speed", &camera->GetSpeed(), 0.005f, 0.0f, 5.0f);
				ImGui::Checkbox("Move Lock X?", &camera->GetTransform()->GetLockedPos(0));
				ImGui::Checkbox("Move Lock Y?", &camera->GetTransform()->GetLockedPos(1));
				ImGui::Checkbox("Move Lock Z?", &camera->GetTransform()->GetLockedPos(2));
				if (!camera->GetTransform()->GetLockedPos(0) || !camera->GetTransform()->GetLockedPos(1) || !camera->GetTransform()->GetLockedPos(2))
				{
					XMFLOAT3 camPos; XMStoreFloat3(&camPos, this->camera->GetTransform()->GetPosition());
					if (!camera->GetTransform()->GetLockedPos(0))
					{
						if (!camera->GetTransform()->GetLockedPos(1))
						{
							if (!camera->GetTransform()->GetLockedPos(2))
								ImGui::DragFloat3("Position XYZ", &camPos.x, 0.1f, -80.0f, 80.0f);
							else
								ImGui::DragFloat2("Position XY", &camPos.x, 0.1f, -80.0f, 80.0f);
						}
						else
						{
							ImGui::DragFloat("Position X", &camPos.x, 0.1f, -80.0f, 80.0f);
							if (!camera->GetTransform()->GetLockedPos(2))
								ImGui::DragFloat("Position Z", &camPos.z, 0.1f, -80.0f, 80.0f);
						}
					}
					else
					{
						if (!camera->GetTransform()->GetLockedPos(1))
						{
							if (!camera->GetTransform()->GetLockedPos(2))
								ImGui::DragFloat2("Position YZ", &camPos.y, 0.1f, -80.0f, 80.0f);
							else
								ImGui::DragFloat("Position Y", &camPos.y, 0.1f, -80.0f, 80.0f);
						}
						else
							ImGui::DragFloat("Position Z", &camPos.z, 0.1f, -80.0f, 80.0f);
					}
					this->camera->GetTransform()->SetPosition(camPos);
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
				ImGui::Checkbox("Draw?", &currentObject->GetAppearance()->GetRenderMode());
				if (currentObject->GetAppearance()->GetRenderMode())
				{
					XMFLOAT3 objPos; XMStoreFloat3(&objPos,this->currentObject->GetTransform()->GetPosition());
					ImGui::DragFloat3("Position XYZ", &objPos.x, 0.1f, -500.0f, 500.0f);
					this->currentObject->GetTransform()->SetPosition(objPos);
					XMFLOAT4 objQ; XMStoreFloat4(&objQ, this->currentObject->GetTransform()->GetOrientation());
					ImGui::DragFloat3("Rotational Axis", &objQ.x, 0.01f -0.5f, 0.5f);
					ImGui::DragFloat("Rotational Angle", &objQ.w, 0.01f, -XM_PI, XM_PI);
					this->currentObject->GetTransform()->SetOrientation(XMLoadFloat4(&objQ));
					ImGui::DragFloat("Scale", &currentObject->GetTransform()->GetScale(), 0.01f, 0.01f, 10.0f);
					ImGui::DragFloat("Damping", &currentObject->GetPhysics()->GetRotationDamping(), 0.001f, 0.0f, 1.0f);
					ImGui::SliderInt("Grayscale Mode", &currentObject->GetAppearance()->GetGrayscale(), 0, 2);
					ImGui::Checkbox("Wireframe?", &currentObject->GetAppearance()->GetWireframeMode());
					ImGui::Checkbox("Use Normal Map?", &currentObject->GetAppearance()->GetNormalMapMode()); ImGui::SameLine();
					ImGui::Checkbox("Use Specular Map?", &currentObject->GetAppearance()->GetSpecularMapMode());
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
				ImGui::SameLine();
				if (ImGui::Button("Move To Camera"))
				{
					currentObject->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + camera->GetTransform()->GetForwardVector() * 10);
				}
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
			ImGui::Checkbox("Show Lights?", &showLights); ImGui::SameLine();
			ImGui::Checkbox("Gravity Enabled", &gravityEnabled);
			if (gravityEnabled)
				ImGui::DragFloat3("Gravity Direction", &GRAVITY.m128_f32[0], 0.00001f, -1.0f, 1.0f);
			ImGui::DragFloat("Push Strength", &pushStrength, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Floor Height", &floorHeight, 1.0f, -100.0f, 100.0f);
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
		ImGui::Checkbox("Show 3D Picking Debug?", &showDebugWindow);
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
	currentObject = objects[objectID].second;
}

void GraphicsHandler::NewCamera()
{
	Camera* camera = new Camera();		
	camera->Init(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), 70.0f, 1.77777779, 0.1f, 1000.0f);
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
	for(int i = 0; i < objects.size(); ++i)
	{
		delete objects[i].second;
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
	PropObject* newObject = new PropObject();
	std::string directory = StringTools::GetDirectoryFromPath(newObjectPath);
	std::string fileExtension = StringTools::GetFileExtension(newObjectPath);
	try
	{
		std::string name = std::string(newObjectPath).substr(directory.length() + 1, std::string(newObjectPath).length() - directory.length() - fileExtension.length() - 2);
		if (newObject->Init(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), newObjectPath, this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader, name))
		{
			objects.insert(objects.begin() + objectID, { newObject->GetName(), newObject });
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
	PropObject* object = new PropObject(*currentObject);
	object->GetName() += " - Copy";
	objects.insert(objects.begin() + objectID, { object->GetName(), object });
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

		LoadScene("data//scenes//" + std::string(sceneName));
		skybox = new PropObject();
		floor = new PropObject();
		debugSphere = new Model();
		debugCube = new Model();
		skybox->Init(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), "data//skies//desert.fbx", this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader,"SKYBOX");
		debugSphere->Init("data//objects//sphere.fbx", this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader);
		debugCube->Init("data//objects//cube.fbx", this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader);
		floor->Init(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), "data//objects//plane.fbx", this->device.Get(), this->deviceContext.Get(), cb_vertexShader, cb_pixelShader,"FLOOR");
		floor->GetTransform()->SetScale(1000.0f);

		currentObject = objects[0].second;
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}


void GraphicsHandler::ResolvePenetrations()
{
	////NEEDS OPTIMIZING
	for (size_t i = 0; i < objects.size(); ++i)
	{
		for (size_t j = 0; j < objects.size(); ++j)
		{
			if (j != i)
			{
				bool collision = CollisionHandler::Instance()->SphereSphereCollision(objects[i].second->GetTransform()->GetPosition(), objects[i].second->GetAppearance()->GetModel()->objectBoundingSphereRadius, objects[j].second->GetTransform()->GetPosition(), objects[j].second->GetAppearance()->GetModel()->objectBoundingSphereRadius);
				if (collision)
				{
					std::string output = "Collision Detected between: " + objects[i].first + " & " + objects[j].first + ".\n";
					//OutputDebugString(output.c_str());
				}
			}
		}
	}
}