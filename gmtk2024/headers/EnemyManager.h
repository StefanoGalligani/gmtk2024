#pragma once

#include <sgEngine.h>
#include <Enemies.h>
#include <Player.h>
#include <irrKlang.h>
using namespace irrklang;

#define SMALLSPHERE 1
#define LARGESPHERE 2
#define LARGEICO 3
#define SHOOTER 4

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
	SphereEnemy* _templateIco;
	Bacteria* _templateBacteria;
	glm::vec3 _spawnPoints[7];
	ISoundEngine* _soundEngine;

	void AddEnemy(int type, glm::vec3 pos) {
		AbstractEnemy* enemy = NULL;
		switch (type) {
		case SMALLSPHERE:
			enemy = new SphereEnemy(_templateSphere);
			break;
		case LARGESPHERE:
			enemy = new LargeEnemy(0, 1, 5, _virusSphereLargeModel, 0.2f, 4, _templateSphere, 2, _renderer, this);
			break;
		case LARGEICO:
			enemy = new SplittingEnemy(8, 4, 30, _virusIcoLargeModel, _templateIco, _renderer, this, _player->GetObject());
			break;
		case SHOOTER:
			enemy = new ShooterEnemy(_virusShooterModel, 3.0f, _templateBacteria, _renderer, this, _player->GetObject());
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
	EnemyManager(sg::Renderer* renderer, Player* player, ISoundEngine* soundEngine) : Entity3D() {
		_renderer = renderer;
		_player = player;
		_soundEngine = soundEngine;

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
		_bacteriaModel->LoadFromObj("res/models/bacteria.obj");

		_templateSphere = new SphereEnemy(10, 5, 20, _virusSphereModel, _player->GetObject());
		_templateIco = new SphereEnemy(8, 4, 30, _virusIcoSmallModel, _player->GetObject());
		_templateBacteria = new Bacteria(30, 0.2f, glm::vec3(0), _bacteriaModel, _player->GetObject());

		_renderer->AddEntity(this);

		InitSpawnPoints();

		AddEnemy(SHOOTER, glm::vec3(20, 0, 20));
	}

	void AttackEnemies(glm::vec3 position, glm::vec3 direction, int length) {
		std::vector<sg::Entity3D*> killedEnemies;
		for (const auto& child : _children) {
			AbstractEnemy* enemy = dynamic_cast<AbstractEnemy*>(child);
			bool killed = enemy->TestHit(position, direction, length);
			if (killed) {
				killedEnemies.push_back(child);
			}
		}
		for (const auto& child : killedEnemies) {
			_children.remove(child);
			_renderer->RemoveObject(dynamic_cast<sg::Object3D*>(child));
			delete(child);
		}
	}

	void Update(double dt) override {
		bool playerHit = false;
		std::vector<sg::Entity3D*> killedEnemies;

		for (const auto& child : _children) {
			AbstractEnemy* enemy = dynamic_cast<AbstractEnemy*>(child);
			glm::vec3 pToEnemy = enemy->GetGlobalPosition() - _player->GetObject()->GetGlobalPosition();
			float distance = glm::length(pToEnemy);
			if (distance <= enemy->_radius + _player->radius) {
				playerHit = true;
				glm::vec3 newEnemyVelocity = enemy->_velocity - 2.0f * glm::dot(enemy->_velocity, pToEnemy) / distance + _player->_velocity;
				glm::vec3 pDeltaVelocity = -2.0f * glm::dot(_player->_velocity, -pToEnemy) / distance + enemy->_velocity;
				enemy->_velocity = newEnemyVelocity;
				_player->_velocity += pDeltaVelocity;
				if (enemy->Damage(1)) killedEnemies.push_back(child);
			}
		}

		if (playerHit) {
			_player->Damage();
			_soundEngine->setSoundVolume(1);
			_soundEngine->play2D("res/sfx/hit.wav");
		}
		for (const auto& child : killedEnemies) {
			_children.remove(child);
			_renderer->RemoveObject(dynamic_cast<sg::Object3D*>(child));
			delete(child);
		}
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
		delete(_templateIco);
	}
};
