#pragma once

#include <sgEngine.h>
#include <glm/glm/gtc/random.hpp>

class EnemyManager : public sg::Entity3D {
private:
	sg::Entity3D* _player;
	sg::Model* _enemyModel;
	sg::Renderer* _renderer;
	float _speed;
	glm::vec2 _spawnPoints[7];

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

	void InitSpawnPoints() {
		_spawnPoints[0] = glm::vec2(-25, -25);
		_spawnPoints[1] = glm::vec2(25, -25);
		_spawnPoints[2] = glm::vec2(-35, 25);
		_spawnPoints[3] = glm::vec2(35, 0);
		_spawnPoints[4] = glm::vec2(35, 20);
		_spawnPoints[5] = glm::vec2(-15, 45);
		_spawnPoints[6] = glm::vec2(15, 45);
	}

	void SpawnZombies(int n) {
		int prevRand = -1;
		for (int i = 0; i < n; i++) {
			int rand = -1;
			do {
				rand = glm::linearRand(0, 6);
			} while (rand == prevRand);
			glm::vec2 pos = _spawnPoints[rand];
			AddEnemy(pos.x, pos.y);
		}
	}

public:
	EnemyManager(sg::Renderer* renderer, float speed, sg::Entity3D* player, const char* modelPath) : Entity3D() {
		_renderer = renderer;
		_player = player;
		_enemyModel = new sg::Model();
		_enemyModel->LoadFromObj(modelPath);
		_speed = speed;

		_renderer->AddEntity(this);

		InitSpawnPoints();

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
			SpawnZombies(2);
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
