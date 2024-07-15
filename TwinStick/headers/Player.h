#pragma once

#include <sgEngine.h>

class Player : public sg::Entity3D {
private:
	sg::Object3D* _playerObj;
	sg::SpotLight3D* _spotLight;
	sg::Camera3D* _mainCamera;
	glm::vec3 _rawVelocity = glm::vec3(0);
	glm::vec3 _velocity = glm::vec3(0);
	bool _pressedKeys[4];
	float _speed;

public:
	Player(sg::Renderer* renderer, float speed, int shadowResx, int shadowResy, int resx, int resy) {
		_speed = speed;

		_playerObj = new sg::Object3D();
		_playerObj->LoadModelFromObj("res/models/player.obj");
		_playerObj->Lit = true;
		_playerObj->CastsShadows = true;
		_playerObj->ReceivesShadows = true;
		_playerObj->PerformFrustumCheck = false;

		_spotLight = new sg::SpotLight3D();
		_spotLight->SetPerspective(1.0f, (float)shadowResx / shadowResy, 0.01f, 100.0f);
		_spotLight->SetGlobalPosition(glm::vec3(-0.2f, 1.18f, -1.27f));
		renderer->SetupShadows(_spotLight, shadowResx, shadowResy);

		_mainCamera = new sg::Camera3D();
		_mainCamera->SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
		_mainCamera->SetGlobalPosition(glm::vec3(0, 9, 6));
		_mainCamera->LookAtGlobal(glm::vec3(0, 0, 0));

		AddChild(_playerObj, false);
		AddChild(_mainCamera, false);
		_playerObj->AddChild(_spotLight, false);

		renderer->AddObject(_playerObj);
		renderer->SetMainCamera(_mainCamera);
		renderer->SetSpotLight(_spotLight);
		renderer->AddEntity(this);

	}

	void SetHoriz(int dir, bool pressed) {
		if (dir == -1) _pressedKeys[0] = pressed;
		if (dir == 1) _pressedKeys[1] = pressed;
		_rawVelocity.x = (_pressedKeys[0] ? -1: 0) + (_pressedKeys[1] ? 1 : 0);
		_velocity = (glm::length2(_rawVelocity) > 0.001) ? glm::normalize(_rawVelocity) : glm::vec3(0);
	}

	void SetVert(int dir, bool pressed) {
		if (dir == -1) _pressedKeys[2] = pressed;
		if (dir == 1) _pressedKeys[3] = pressed;
		_rawVelocity.z = (_pressedKeys[2] ? -1 : 0) + (_pressedKeys[3] ? 1 : 0);
		_velocity = (glm::length2(_rawVelocity) > 0.001) ? glm::normalize(_rawVelocity) : glm::vec3(0);
	}

	void SetNewDirection(int x, int z) {
		_playerObj->LookAtLocal(glm::normalize(glm::vec3(x, 0, z)));
	}

	void Update(double dt) override {
		sg::Entity3D::Update(dt);
		TranslateGlobal((float)dt * _velocity * _speed);
		_globalTransform.position = glm::clamp(GetGlobalPosition(), glm::vec3(-26, 0, -17), glm::vec3(26, 0, 35));
	}

	void UpdateCameraResolution(int resx, int resy) {
		_mainCamera->SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
	}

	glm::vec3 GetDirection() {
		return _playerObj->GlobalForward();
	}

	~Player() {
		delete(_playerObj);
		delete(_spotLight);
		delete(_mainCamera);
	}
};