#pragma once
#include <sgObject3D.h>
#include <glm/glm/gtx/vector_angle.hpp>
#include <glm/glm/gtc/random.hpp>

glm::vec3 RandomVec3() {
	return glm::linearRand(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
}

int Random(int min, int max) {
	return glm::linearRand(min, max-1);
}

class AbstractEnemy : public sg::Object3D {
private:
	int _health;

protected:
	virtual glm::vec3 GetDirection() {
		return glm::vec3(0);
	}

	virtual glm::vec3 GetCenterOffset() {
		return glm::vec3(0);
	}

	virtual void BeforeDeath() {
	}

	float _acceleration;
	float _deceleration;
	float _maxVelocity;

public:
	AbstractEnemy(int health, float radius, float acceleration, float deceleration, float maxVelocity, sg::Model* model) : Object3D() {
		_health = health;
		_radius = radius;
		_acceleration = acceleration;
		_deceleration = deceleration;
		_maxVelocity = maxVelocity;
		SetModel(model);
		Lit = true;
		CastsShadows = true;
		ReceivesShadows = true;
	}

	bool Damage(int dmg) {
		_health -= dmg;
		if (_health <= 0) {
			BeforeDeath();
			return true;
		}
		return false;
	}

	bool TestHit(glm::vec3 origin, glm::vec3 direction, int length) {
		glm::vec3 toTarget = (GetGlobalPosition() + GetCenterOffset()) - origin;
		float distance = glm::length(toTarget);
		if (distance > length) return false;
		float cosa = glm::dot(toTarget, direction) / distance;
		if (cosa <= 0) return false;
		float sina = glm::sqrt(1 - cosa * cosa);
		float horizDistance = distance *sina / cosa;
		bool hit = horizDistance <= _radius;
		if (hit) {
			return Damage(1);
		}
		return false;
	}

	void Update(double dt) override {
		sg::Object3D::Update(dt);
		if (glm::length2(_velocity) > 0.01f) _velocity -= glm::normalize(_velocity) * _deceleration * float(dt);
		else _velocity = glm::vec3(0);

		_velocity += GetDirection() * _acceleration * float(dt);

		if (glm::length2(_velocity) > _maxVelocity * _maxVelocity) _velocity = glm::normalize(_velocity) * _maxVelocity;
		TranslateGlobal((float)dt * _velocity);
	}

	glm::vec3 _velocity;
	float _radius;
};

class SphereEnemy : public AbstractEnemy {
private:
	sg::Object3D* _playerObj;

protected:
	virtual glm::vec3 GetDirection() {
		return glm::normalize(_playerObj->GetGlobalPosition() - GetGlobalPosition());
	}

public:
	SphereEnemy(float acceleration, float deceleration, float maxVelocity, sg::Model* model, sg::Object3D* playerObj)
		: AbstractEnemy(5, 1.5f, acceleration, deceleration, maxVelocity, model) {
		_playerObj = playerObj;
	}

	SphereEnemy(SphereEnemy* temp)
		: SphereEnemy(temp->_acceleration, temp->_deceleration, temp->_maxVelocity, temp->GetModel(), temp->_playerObj) { }

	void Update(double dt) override {
		AbstractEnemy::Update(dt);
	}
};

class LargeEnemy : public AbstractEnemy {
private:
	glm::vec3 _rotAxis;
	float _rotSpeed;
	float _cooldownTime;
	float _counter;
	SphereEnemy* _templateEnemy;
	int _nEnemies;
	std::vector<glm::vec3> _spawnPoints;
	sg::Renderer* _renderer;
	sg::Entity3D* _enemyManager;

	void InitSpawnPoints() {
		_spawnPoints.resize(2);
		_spawnPoints[0] = glm::vec3(1, 1, 1);
		_spawnPoints[1] = glm::vec3(1, 1, -1);
	}

	void SpawnEnemies() {
		for (int i = 0; i < _nEnemies; i++) {
			SphereEnemy* s = new SphereEnemy(_templateEnemy);
			glm::vec3 pos = _spawnPoints[Random(0, _spawnPoints.size())];
			glm::vec3 offset = GlobalForward() * pos.z + GlobalRight() * pos.x + GlobalUp() * pos.y;
			s->SetGlobalPosition(GetGlobalPosition() + offset);
			_enemyManager->AddChild(s, false);
			_renderer->AddObject(s);
		}
	}

protected:
	virtual glm::vec3 GetDirection() {
		return glm::vec3(0);
	}

public:
	LargeEnemy(float acceleration, float deceleration, float maxVelocity, sg::Model* model,
		float rotSpeed, float cooldownTime, SphereEnemy* templateEnemy, int nEnemies, sg::Renderer* renderer, sg::Entity3D* enemyManager)
		: AbstractEnemy(10, 7.5f, acceleration, deceleration, maxVelocity, model) {
		_cooldownTime = cooldownTime;
		_rotSpeed = rotSpeed;
		_rotAxis = glm::normalize(RandomVec3());
		_templateEnemy = templateEnemy;
		_nEnemies = nEnemies;
		_renderer = renderer;
		_enemyManager = enemyManager;
		InitSpawnPoints();
	}

	void Update(double dt) override {
		RotateGlobal(_rotAxis, _rotSpeed * float(dt));
		_counter += float(dt);
		if (_counter >= _cooldownTime) {
			_counter -= _cooldownTime;
			SpawnEnemies();
		}
	}
};

class SplittingEnemy : public AbstractEnemy {
private:
	SphereEnemy* _templateEnemy;
	sg::Renderer* _renderer;
	sg::Entity3D* _enemyManager;
	sg::Object3D* _playerObj;

	void SpawnEnemies() {
		for (int i = 0; i < 2; i++) {
			SphereEnemy* s = new SphereEnemy(_templateEnemy);
			glm::vec3 offset = 4.0f * RandomVec3();
			s->SetGlobalPosition(GetGlobalPosition() + offset);
			s->_velocity = _velocity + 5.0f * (s->GetGlobalPosition() - GetGlobalPosition());
			_enemyManager->AddChild(s, false);
			_renderer->AddObject(s);
		}
	}

protected:
	virtual glm::vec3 GetDirection() {
		return glm::normalize(_playerObj->GetGlobalPosition() - GetGlobalPosition());
	}

	virtual void BeforeDeath() override {
		SpawnEnemies();
	}

public:
	SplittingEnemy(float acceleration, float deceleration, float maxVelocity, sg::Model* model,
		SphereEnemy* templateEnemy, sg::Renderer* renderer, sg::Entity3D* enemyManager, sg::Object3D* playerObj)
		: AbstractEnemy(4, 2.8f, acceleration, deceleration, maxVelocity, model) {
		_renderer = renderer;
		_enemyManager = enemyManager;
		_templateEnemy = templateEnemy;
		_playerObj = playerObj;
	}

	void Update(double dt) override {
		AbstractEnemy::Update(dt);
	}
};

class Bacteria : public AbstractEnemy { //aggiungere Time to live, con cambiamento di scala verso lo 0
private:
	glm::vec3 _direction;

protected:
	virtual glm::vec3 GetDirection() override {
		return _direction;
	}

	virtual glm::vec3 GetCenterOffset() override {
		return GlobalForward() * 1.5f;
	}

	float _rotSpeed;
	sg::Object3D* _playerObj;

public:
	Bacteria(float maxVelocity, float rotSpeed, glm::vec3 initialPosition, sg::Model* model, sg::Object3D* playerObj)
		: AbstractEnemy(1, 1.5f, 100, 0, maxVelocity, model) {
		_rotSpeed = rotSpeed;
		_playerObj = playerObj;
		_direction = glm::normalize(_playerObj->GetGlobalPosition() - initialPosition);
	}

	Bacteria(Bacteria* b, glm::vec3 initialPosition)
		: Bacteria(b->_maxVelocity, b->_rotSpeed, initialPosition, b->GetModel(), b->_playerObj) {
	}

	void Update(double dt) override {
		_direction += glm::normalize(_playerObj->GetGlobalPosition() - GetGlobalPosition());
		AbstractEnemy::Update(dt);
		LookAtGlobal(_direction);
	}
};

class ShooterEnemy : public AbstractEnemy {
private:
	Bacteria* _templateEnemy;
	sg::Renderer* _renderer;
	sg::Entity3D* _enemyManager;
	sg::Object3D* _playerObj;
	float _cooldownTime;
	float _counter;

	void SpawnBullet() {
		glm::vec3 pos = GetGlobalPosition() + GlobalUp() * 7.0f;
		Bacteria* b = new Bacteria(_templateEnemy, pos);
		b->SetGlobalPosition(pos);
		_enemyManager->AddChild(b, false);
		_renderer->AddObject(b);
	}

protected:

	virtual glm::vec3 GetCenterOffset() override {
		return GlobalUp() * 3.5f;
	}

public:
	ShooterEnemy(sg::Model* model, float cooldownTime, Bacteria* templateEnemy, sg::Renderer* renderer, sg::Entity3D* enemyManager, sg::Object3D* playerObj)
		: AbstractEnemy(4, 2.8f, 0, 0, 0, model) {
		_cooldownTime = cooldownTime;
		_renderer = renderer;
		_enemyManager = enemyManager;
		_templateEnemy = templateEnemy;
		_playerObj = playerObj;
	}

	void Update(double dt) override {
		AbstractEnemy::Update(dt);
		_counter += float(dt);
		if (_counter >= _cooldownTime) {
			_counter -= _cooldownTime;
			SpawnBullet();
		}
	}
};