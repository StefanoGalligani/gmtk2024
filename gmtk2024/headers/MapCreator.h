#pragma once

#include <sgEngine.h>

class MapCreator {
private:
    sg::Renderer* _renderer;
    sg::Object3D* _mapObj;
    std::vector<sg::Plane> _planes;

    void InitPlanes() {
        _planes.push_back(sg::Plane(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)));
        _planes.push_back(sg::Plane(glm::vec3(0,54, 0), glm::vec3(0, -1, 0)));
        _planes.push_back(sg::Plane(glm::vec3(-193, 0, 0), glm::vec3(1, 0, 0)));
        _planes.push_back(sg::Plane(glm::vec3(55, 0, 0), glm::vec3(-1, 0, 0)));
        _planes.push_back(sg::Plane(glm::vec3(0, 0, 120), glm::vec3(0, 0, -1)));
        _planes.push_back(sg::Plane(glm::vec3(0, 0, -94), glm::vec3(0, 0, 1)));
        _planes.push_back(sg::Plane(glm::vec3(30, 0, -60), glm::vec3(-1, 0, 1)));
        _planes.push_back(sg::Plane(glm::vec3(40, 0, 106), glm::vec3(-1, 0, -1)));
        _planes.push_back(sg::Plane(glm::vec3(-220, 0, -40), glm::vec3(1, 0, 1)));
        _planes.push_back(sg::Plane(glm::vec3(-150, 0, 75), glm::vec3(1, 0, -1)));
    }

public:
	MapCreator(sg::Renderer* renderer) {
        _renderer = renderer;
        _mapObj = new sg::Object3D();

        _mapObj->LoadModelFromObj("res/models/stomach.obj");
        _mapObj->Lit = true;
        _mapObj->ReceivesShadows = true;
        _mapObj->PerformFrustumCheck = false;

        renderer->AddObject(_mapObj);

        InitPlanes();
	}

    glm::vec3 GetValidPosition(glm::vec3 oldPos) {
        for (int i = 0; i < _planes.size(); i++) {
            sg::Plane p = _planes[i];
            float distance = glm::dot(oldPos, p.normal) - p.distance;
            if (distance < 0) {
                oldPos += p.normal * -distance;
            }
        }
        return oldPos;
    }

	~MapCreator() {
        delete(_mapObj);
	}
};