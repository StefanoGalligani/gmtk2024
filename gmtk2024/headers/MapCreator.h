#pragma once

#include <sgEngine.h>
#include <glm/glm/gtc/random.hpp>

class MapCreator {
private:
    sg::Renderer* _renderer;

    sg::Object3D* _mapObj;

public:
	MapCreator(sg::Renderer* renderer) {
        _renderer = renderer;
        _mapObj = new sg::Object3D();

        _mapObj->LoadModelFromObj("res/models/stomach.obj");
        _mapObj->Lit = true;
        _mapObj->ReceivesShadows = true;
        _mapObj->PerformFrustumCheck = false;

        renderer->AddObject(_mapObj);
	}

	~MapCreator() {
        delete(_mapObj);
	}
};