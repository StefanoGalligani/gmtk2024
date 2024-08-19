#pragma once

#include <sgEngine.h>
#include <MapCreator.h>

class Player : public sg::Entity3D {
private:
	sg::Object3D* _playerObj;
	sg::SpotLight3D* _spotLightRight;
	sg::SpotLight3D* _spotLightLeft;
	sg::Camera3D* _mainCamera;
	sg::Object3D* _rayObj;
	sg::Renderer* _renderer;
	MapCreator* _mapCreator;
	bool _pressedAccel;
	float _acceleration;
	float _deceleration;
	float _maxVelocity;
	float _health;
	float _maxHealth;
	float _regenSpeed;
	float _invincibility;
	int _rayShown;
	float _baseColor[3];

public:
	Player(sg::Renderer* renderer, int health, float regenSpeed, float acceleration, float deceleration, float maxVelocity, int shadowResx, int shadowResy, int resx, int resy, MapCreator* mapCreator) {
		_acceleration = acceleration;
		_deceleration = deceleration;
		_maxVelocity = maxVelocity;
		_maxHealth = float(health);
		_health = float(health);
		_regenSpeed = regenSpeed;

		_mapCreator = mapCreator;

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

		_rayObj = new sg::Object3D();
		_rayObj->LoadModelFromObj("res/models/ray.obj");

		AddChild(_playerObj, false);
		AddChild(_mainCamera, false);
		_playerObj->AddChild(_spotLightRight, false);
		_playerObj->AddChild(_spotLightLeft, false);
		_playerObj->AddChild(_rayObj, false);

		_renderer = renderer;

		renderer->AddObject(_playerObj);
		renderer->SetMainCamera(_mainCamera);

		renderer->AddLight(_spotLightRight);
		renderer->AddLight(_spotLightLeft);
		renderer->AddEntity(this);

		sg::Material* mat = _playerObj->GetMaterialReferenceAt(1);
		_baseColor[0] = mat->Kd[0];
		_baseColor[1] = mat->Kd[1];
		_baseColor[2] = mat->Kd[2];

		SetGlobalPosition(0, 5, 0);
	}

	sg::Object3D* GetObject() {
		return _playerObj;
	}

	void ShowRay() {
		_rayObj->LookAtGlobal(_rayObj->GetGlobalPosition() + _playerObj->GlobalForward());
		_renderer->AddObject(_rayObj);
		_rayShown = 2;
	}

	bool IsDead() {
		return _health <= 0;
	}

	void SetAccel(bool pressed) {
		_pressedAccel = pressed;
	}

	void ChangeDirection(float x, float y) {
		RotateGlobal(glm::vec3(0, 1, 0), _playerObj->GlobalUp().y * x);
		_playerObj->RotateLocal(glm::vec3(1, 0, 0), y);

		_mainCamera->SetGlobalPosition(_playerObj->GetGlobalPosition() + _playerObj->GlobalUp() - _playerObj->GlobalForward() * 2.0f);
		_mainCamera->LookAtGlobal(_playerObj->GetGlobalPosition(), _playerObj->GlobalUp());

		_spotLightLeft->SetGlobalPosition(_playerObj->GetGlobalPosition() +
			-0.354049f * _playerObj->GlobalRight() +
			0.238036f * _playerObj->GlobalUp() +
			0.886109f * _playerObj->GlobalForward());
		_spotLightLeft->LookAtGlobal(_spotLightLeft->GetGlobalPosition() + _playerObj->GlobalForward());

		_spotLightRight->SetGlobalPosition(_playerObj->GetGlobalPosition() +
			0.354049f * _playerObj->GlobalRight() +
			0.238036f * _playerObj->GlobalUp() +
			0.886109f * _playerObj->GlobalForward());
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
		SetGlobalPosition(_mapCreator->GetValidPosition(GetGlobalPosition()));

		if (_rayShown > 0) _rayShown--;
		if (_rayShown == 0) {
			_renderer->RemoveObject(_rayObj);
		}

		_invincibility -= float(dt);
		_health = glm::min(_maxHealth, _health + float(dt) * _regenSpeed);
		sg::Material* mat = _playerObj->GetMaterialReferenceAt(1);
		mat->Kd[0] = _baseColor[0] * (_health / _maxHealth);
		mat->Kd[1] = _baseColor[1] * (_health / _maxHealth);
		mat->Kd[2] = _baseColor[2] * (_health / _maxHealth);
	}

	void Damage() {
		if (_invincibility > 0.0f) return;
		_health -= 1;
		_invincibility = 0.3f;
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
		delete(_rayObj);
	}

	float radius = 1.2f;
	glm::vec3 _velocity = glm::vec3(0);
};