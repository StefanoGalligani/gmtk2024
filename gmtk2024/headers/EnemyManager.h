#pragma once

#include <sgEngine.h>
#include <Enemies.h>
#include <Player.h>

#define SMALLSPHERE 1
#define LARGESPHERE 2
#define SMALLICO 3
#define LARGEICO 4
#define SHOOTER 5

class EnemyManager : public sg::Entity3D {
private:
	Player* _player;
	sg::Model* _virusSphereModel;
	sg::Model* _virusSphereLargeModel;
	sg::Model* _virusIcoLargeModel;
	sg::Model* _virusIcoSmallModel;
	sg::Model* _virusShooterModel;
	sg::Model* _bacteriaModel;
	sg::Renderer* _renderer;
	SphereEnemy* _templateSphere;
	glm::vec3 _spawnPoints[7];

	void AddEnemy(int type, glm::vec3 pos) {
		AbstractEnemy* enemy = NULL;
		switch (type) {
		case SMALLSPHERE:
			enemy = new SphereEnemy(_templateSphere);
			break;
		case LARGESPHERE:
			enemy = new LargeEnemy(5, 0, 1, 5, _virusSphereLargeModel, 0.2f, 4, _templateSphere, 2, _renderer, this);
			break;
		case SMALLICO:
			break;
		case LARGEICO:
			break;
		case SHOOTER:
			break;
		default:
			return;
		}
		enemy->SetGlobalPosition(pos);
		enemy->Lit = true;
		enemy->CastsShadows = true;
		enemy->ReceivesShadows = true;
		AddChild(enemy, true);
		_renderer->AddObject(enemy);
	}

	void InitSpawnPoints() {
		_spawnPoints[0] = glm::vec3(-25, 0, 25);
		_spawnPoints[1] = glm::vec3(25, 0, 25);
		_spawnPoints[2] = glm::vec3(-35, 0, -25);
		_spawnPoints[3] = glm::vec3(35, 0, 0);
		_spawnPoints[4] = glm::vec3(35, 0, -20);
		_spawnPoints[5] = glm::vec3(-15, 0, -45);
		_spawnPoints[6] = glm::vec3(15, 0, -45);
	}

	void SpawnEnemy(int type, int n) {
		int prevRand = -1;
		for (int i = 0; i < n; i++) {
			int rand = -1;
			do {
				rand = glm::linearRand(0, 6);
			} while (rand == prevRand);
			glm::vec3 pos = _spawnPoints[rand];
			AddEnemy(type, pos);
		}
	}

public:
	EnemyManager(sg::Renderer* renderer, Player* player) : Entity3D() {
		_renderer = renderer;
		_player = player;

		_virusSphereModel = new sg::Model();
		_virusSphereModel->LoadFromObj("res/models/virus_sphere.obj");
		_virusSphereLargeModel = new sg::Model();
		_virusSphereLargeModel->LoadFromObj("res/models/virus_large.obj");
		_virusIcoLargeModel = new sg::Model();
		_virusIcoLargeModel->LoadFromObj("res/models/virus_ico_large.obj");
		_virusIcoSmallModel = new sg::Model();
		_virusIcoSmallModel->LoadFromObj("res/models/virus_ico_small.obj");
		_virusShooterModel = new sg::Model();
		_virusShooterModel->LoadFromObj("res/models/virus_shooter.obj");
		_bacteriaModel = new sg::Model();
		_bacteriaModel->LoadFromObj("bacteria.obj");

		_templateSphere = new SphereEnemy(5, 10, 5, 20, _virusSphereModel, _player->GetObject());

		_renderer->AddEntity(this);

		InitSpawnPoints();

		AddEnemy(LARGESPHERE, glm::vec3(20, 20, 20));
	}

	void Update(double dt) override {
	}

	bool CheckCollision(glm::vec3 position) {
		return false;
	}

	~EnemyManager() {
		for (const auto& child : _children) {
			delete(child);
		}

		delete(_virusSphereModel);
		delete(_virusSphereLargeModel);
		delete(_virusIcoLargeModel);
		delete(_virusIcoSmallModel);
		delete(_virusShooterModel);
		delete(_bacteriaModel);
		delete(_templateSphere);
	}
};
