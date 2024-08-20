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

struct Wave {
	std::vector<int> enemies;
	std::vector<int> count;
	std::vector<int> spawnPoints;
};

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
	std::vector<glm::vec3> _spawnPoints;
	ISoundEngine* _soundEngine;

	int _waveIndex = 0;
	std::vector<Wave> _waves;

	void InitSpawnPoints() {
		_spawnPoints.push_back(glm::vec3(7, 0.5f, -85));
		_spawnPoints.push_back(glm::vec3(10, 0, 18));
		_spawnPoints.push_back(glm::vec3(-95, 0.3f, 50));
		_spawnPoints.push_back(glm::vec3(-159, 0, -0.8f));
		_spawnPoints.push_back(glm::vec3(-87, 4, -65));

		_spawnPoints.push_back(glm::vec3(8.5f, 30, -33.5f));
		_spawnPoints.push_back(glm::vec3(-42.5f, 30, 34));
		_spawnPoints.push_back(glm::vec3(-127, 30, 25));
		_spawnPoints.push_back(glm::vec3(-123, 30, -33));
		_spawnPoints.push_back(glm::vec3(-40, 30, -75));

		_spawnPoints.push_back(glm::vec3(-20, 25, -15));
		_spawnPoints.push_back(glm::vec3(-80, 25, 0));
	}

	void AddToWave(Wave &wave, int type, int n, int spawnPoint) {
		wave.enemies.push_back(type);
		wave.count.push_back(n);
		wave.spawnPoints.push_back(spawnPoint);
	}

	void InitWaves() {
		Wave w1;
		AddToWave(w1, SMALLSPHERE, 1, 9);
		AddToWave(w1, SMALLSPHERE, 1, 7);
		AddToWave(w1, SMALLSPHERE, 1, 8);

		Wave w2;
		AddToWave(w2, SMALLSPHERE, 1, 6);
		AddToWave(w2, LARGEICO, 1, 7);
		AddToWave(w2, SMALLSPHERE, 1, 8);
		AddToWave(w2, LARGEICO, 1, 9);
		AddToWave(w2, SMALLSPHERE, 1, 10);

		Wave w3;
		AddToWave(w3, LARGEICO, 1, 6);
		AddToWave(w3, LARGEICO, 1, 7);
		AddToWave(w3, LARGEICO, 1, 8);
		AddToWave(w3, LARGEICO, 1, 9);
		AddToWave(w3, SHOOTER, 1, 0);

		Wave w4;
		AddToWave(w4, SHOOTER, 1, 1);
		AddToWave(w4, SHOOTER, 1, 2);
		AddToWave(w4, SHOOTER, 1, 4);

		Wave w5;
		AddToWave(w5, LARGESPHERE, 1, 10);

		Wave w6;
		AddToWave(w6, SHOOTER, 1, 0);
		AddToWave(w6, SHOOTER, 1, 3);
		AddToWave(w6, LARGESPHERE, 1, 10);
		AddToWave(w6, LARGESPHERE, 1, 11);
		AddToWave(w6, LARGEICO, 1, 6);
		AddToWave(w6, LARGEICO, 1, 7);
		AddToWave(w6, LARGEICO, 1, 9);

		_waves.push_back(w1);
		_waves.push_back(w2);
		_waves.push_back(w3);
		_waves.push_back(w4);
		_waves.push_back(w5);
		_waves.push_back(w6);
	}

	void AddEnemy(int type, glm::vec3 pos) {
		AbstractEnemy* enemy = NULL;
		switch (type) {
		case SMALLSPHERE:
			enemy = new SphereEnemy(_templateSphere);
			break;
		case LARGESPHERE:
			enemy = new LargeEnemy(0, 1, 5, _virusSphereLargeModel, 0.2f, 3, _templateSphere, 2, _renderer, this);
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
		if (glm::length(pos - _player->GetObject()->GetGlobalPosition()) < 5) {
			pos -= glm::normalize(pos) * 5.0f;
		}
		enemy->SetGlobalPosition(pos);
		enemy->Lit = true;
		enemy->CastsShadows = true;
		enemy->ReceivesShadows = true;
		AddChild(enemy, true);
		_renderer->AddObject(enemy);
	}

	void SpawnEnemy(int type, int n, int spawnPoint) {
		for (int i = 0; i < n; i++) {
			glm::vec3 pos = _spawnPoints[spawnPoint];
			AddEnemy(type, pos);
		}
	}

	void StartWave() {
		if (_waveIndex == _waves.size()) {
			finished = true;
			return;
		}
		Wave w = _waves[_waveIndex];
		for (int i = 0; i < w.spawnPoints.size(); i++) {
			SpawnEnemy(w.enemies[i], w.count[i], w.spawnPoints[i]);
		}
		_waveIndex++;
	}

public:
	bool finished = false;

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

		_templateSphere = new SphereEnemy(10, 5, 25, _virusSphereModel, _player->GetObject());
		_templateIco = new SphereEnemy(10, 6, 20, _virusIcoSmallModel, _player->GetObject());
		_templateBacteria = new Bacteria(30, 0.2f, glm::vec3(0), _bacteriaModel, _player->GetObject());

		_renderer->AddEntity(this);

		InitSpawnPoints();
		InitWaves();

		StartWave();
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
				enemy->Damage(1);
			}
			if (enemy->IsDead()) killedEnemies.push_back(child);
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

		if(!finished && _children.size() == 0) {
			StartWave();
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
