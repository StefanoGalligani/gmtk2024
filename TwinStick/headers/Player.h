#pragma once

#include <sgEngine.h>

class Player : public sg::Entity3D {
private:
	sg::Object3D* _playerObj;
	sg::SpotLight3D* _spotLight;
	sg::Camera3D* _mainCamera;

public:
	Player(sg::Renderer* renderer, int shadowResx, int shadowResy, int resx, int resy) {
		_playerObj = new sg::Object3D();
		_playerObj->LoadModelFromObj("res/models/player.obj");
		_playerObj->Lit = true;
		_playerObj->CastsShadows = true;
		_playerObj->ReceivesShadows = true;
		_playerObj->PerformFrustumCheck = false;
		_playerObj->RotateLocal(_playerObj->LocalUp(), 3.1415926535f);

		_spotLight = new sg::SpotLight3D();
		_spotLight->SetPerspective(1.0f, (float)shadowResx / shadowResy, 0.01f, 100.0f);
		_spotLight->SetGlobalPosition(glm::vec3(-0.2f, 1.18f, -1.27f));
		renderer->SetupShadows(_spotLight, shadowResx, shadowResy);

		_mainCamera = new sg::Camera3D();
		_mainCamera->SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
		_mainCamera->SetGlobalPosition(glm::vec3(0, 8, 5));
		_mainCamera->LookAtGlobal(glm::vec3(0, 0, 0));

		AddChild(_playerObj, false);
		AddChild(_mainCamera, false);
		_playerObj->AddChild(_spotLight, false);

		renderer->AddObject(_playerObj);
		renderer->SetMainCamera(_mainCamera);
		renderer->SetSpotLight(_spotLight);

		_playerObj->RotateGlobal(glm::vec3(0, 1, 0), 3.1415926535f);
	}

	void UpdateCameraResolution(int resx, int resy) {
		_mainCamera->SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
	}

	~Player() {
		delete(_playerObj);
		delete(_spotLight);
		delete(_mainCamera);
	}
};