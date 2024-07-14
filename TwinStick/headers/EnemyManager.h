#pragma once

#include <sgEngine.h>

class EnemyManager : public sg::Entity3D {
private:
	sg::Entity3D* _player;
	sg::Model* _enemyModel;
	sg::Renderer* _renderer;
	float _speed;

	void AddEnemy(float x, float z) {
		sg::Object3D* enemy = new sg::Object3D();
		enemy->SetGlobalPosition(x, 0, z);
		enemy->SetModel(_enemyModel);
		enemy->Lit = true;
		enemy->CastsShadows = true;
		enemy->ReceivesShadows = true;
		AddChild(enemy, true);
		_renderer->AddObject(enemy);
	}

public:
	EnemyManager(sg::Renderer* renderer, float speed, sg::Entity3D* player, const char* modelPath) : Entity3D() {
		_renderer = renderer;
		_player = player;
		_enemyModel = new sg::Model();
		_enemyModel->LoadFromObj(modelPath);
		_speed = speed;

		_renderer->AddEntity(this);

		AddEnemy(0, 15);
		AddEnemy(-15, 0);
	}

	void Update(double dt) override {
		for (const auto& child : _children) {
			child->LookAtGlobal(_player->GetGlobalPosition());
			child->TranslateGlobal((float)dt * _speed * child->GlobalForward());
		}
	}

	bool CheckCollision(glm::vec3 position) {
		sg::Object3D* toDelete = NULL;
		for (const auto& child : _children) {
			if (glm::distance2(child->GetGlobalPosition(), position) < 1) {
				toDelete = dynamic_cast<sg::Object3D*>(child);
				break;
			}
		}
		if (toDelete != NULL) {
			_renderer->RemoveObject(toDelete);
			RemoveChild(toDelete, false);
			delete(toDelete);
			return true;
		}
		return false;
	}

	~EnemyManager() {
		for (const auto& child : _children) {
			delete(child);
		}
	}
};
