#pragma once

#include<sgEngine.h>
#include<EnemyManager.h>

class Bullet : public sg::Object3D {
private:
	glm::vec3 _velocity;
	float _lifetime;
	EnemyManager* _enemyManager;
	sg::Renderer* _renderer;

public:
	Bullet(glm::vec3 position, glm::vec3 direction, float speed, float lifetime, EnemyManager* enemyManager, sg::Renderer* renderer, sg::Model* model) : Object3D() {
		CastsShadows = true;
		ReceivesShadows = true;
		Lit = true;
		SetModel(model);
		SetGlobalPosition(position);
		LookAtGlobal(position + direction);
		_velocity = speed * direction;
		_enemyManager = enemyManager;
		_lifetime = lifetime;
		_renderer = renderer;
	}

	void Update(double dt) override {
		TranslateGlobal((float)dt * _velocity);
		bool enemyHit = true;// _enemyManager->CheckCollision(GetGlobalPosition());
		_lifetime -= (float)dt;
		if (enemyHit || _lifetime < 0) {
			_renderer->RemoveObject(this);
			delete(this);
		}
	}
};