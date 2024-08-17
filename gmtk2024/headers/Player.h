#pragma once

#include <sgEngine.h>

class Player : public sg::Entity3D {
private:
	sg::Object3D* _playerObj;
	sg::SpotLight3D* _spotLightRight;
	sg::SpotLight3D* _spotLightLeft;
	sg::Camera3D* _mainCamera;
	glm::vec3 _velocity = glm::vec3(0);
	bool _pressedAccel;
	float _acceleration;
	float _deceleration;
	float _maxVelocity;
	float _health;

public:
	Player(sg::Renderer* renderer, int health, float acceleration, float deceleration, float maxVelocity, int shadowResx, int shadowResy, int resx, int resy) {
		_acceleration = acceleration;
		_deceleration = deceleration;
		_maxVelocity = maxVelocity;
		_health = float(health);

		_playerObj = new sg::Object3D();
		_playerObj->LoadModelFromObj("res/models/ship.obj");
		_playerObj->Lit = true;
		_playerObj->CastsShadows = true;
		_playerObj->ReceivesShadows = false;
		_playerObj->SetGlobalPosition(0, 0, 0);

		_spotLightRight = new sg::SpotLight3D(shadowResx, shadowResy, 3.14f / 10, (float)shadowResx / shadowResy, 0.05f, 300.0f);
		_spotLightRight->SetGlobalPosition(glm::vec3(0.354049f, 0.238036f, -0.886109f));
		_spotLightRight->SetColor(glm::vec3(1.0, 1.0, 0.8));
		_spotLightRight->SetMapTexture("res/lightMask.jpg");

		_spotLightLeft = new sg::SpotLight3D(shadowResx, shadowResy, 3.14f / 10, (float)shadowResx / shadowResy, 0.05f, 300.0f);
		_spotLightLeft->SetGlobalPosition(glm::vec3(-0.354049f, 0.238036f, -0.886109f));
		_spotLightLeft->SetColor(glm::vec3(1.0, 1.0, 0.8));
		_spotLightLeft->SetMapTexture("res/lightMask.jpg");

		_mainCamera = new sg::Camera3D(1.5f, (float)resx / resy, 0.05f, 3000.0f);
		_mainCamera->SetLocalPosition(glm::vec3(0, 1, 2));
		_mainCamera->LookAtLocal(glm::vec3(0, 0, 0));

		AddChild(_playerObj, false);
		AddChild(_mainCamera, false);
		_playerObj->AddChild(_spotLightRight, false);
		_playerObj->AddChild(_spotLightLeft, false);

		renderer->AddObject(_playerObj);
		renderer->SetMainCamera(_mainCamera);

		renderer->AddLight(_spotLightRight);
		renderer->AddLight(_spotLightLeft);
		renderer->AddEntity(this);
	}

	sg::Object3D* GetObject() {
		return _playerObj;
	}

	bool IsDead() {
		return _health <= 0;
	}

	void SetAccel(bool pressed) {
		_pressedAccel = pressed;
	}

	void ChangeDirection(float x, float y) {
		RotateGlobal(glm::vec3(0, 1, 0), x);
		_playerObj->RotateLocal(glm::vec3(1, 0, 0), y);
		_mainCamera->SetGlobalPosition(_playerObj->GetGlobalPosition() + _playerObj->GlobalUp() - _playerObj->GlobalForward()*2.0f);
		_mainCamera->LookAtGlobal(_playerObj->GetGlobalPosition(), _playerObj->GlobalUp());
		_spotLightLeft->LookAtGlobal(_spotLightLeft->GetGlobalPosition() + _playerObj->GlobalForward());
		_spotLightRight->LookAtGlobal(_spotLightRight->GetGlobalPosition() + _playerObj->GlobalForward());
	}

	void Update(double dt) override {
		sg::Entity3D::Update(dt);
		if (glm::length2(_velocity) > 0.01f) _velocity -= glm::normalize(_velocity) * _deceleration * float(dt);
		else _velocity = glm::vec3(0);
		if (_pressedAccel) {
			_velocity += _playerObj->GlobalForward() * _acceleration * float(dt);
		}
		if (glm::length2(_velocity) > _maxVelocity * _maxVelocity) _velocity = glm::normalize(_velocity) * _maxVelocity;
		TranslateGlobal((float)dt * _velocity);
	}

	void UpdateCameraResolution(int resx, int resy) {
		_mainCamera->SetAspectRatio((float)resx / resy);
	}

	glm::vec3 GetDirection() {
		return _playerObj->GlobalForward();
	}

	~Player() {
		delete(_playerObj);
		delete(_spotLightRight);
		delete(_spotLightLeft);
		delete(_mainCamera);
	}
};