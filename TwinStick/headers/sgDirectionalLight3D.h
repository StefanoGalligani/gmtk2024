#pragma once

#include <sgAngledLight3D.h>

namespace sg {
	class DirectionalLight3D : public AngledLight3D {
	private:
		glm::vec3 _direction;
		float _distance;

		void UpdateCoords() {
			SetLocalPosition(-_direction * _distance);
			LookAtLocal(glm::vec3(0,0,0));
			UpdateView();
		}

	protected:
		void UpdateProjectionMatrix() override {
			SetOrthogonal();
		}

	public:
		DirectionalLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane, glm::vec3 direction = glm::vec3(0, -1, -0.2), float distance = 100)
			: AngledLight3D(width, height, fov, aspectRatio, nearPlane, farPlane) {
			UpdateProjectionMatrix();
			_direction = glm::normalize(direction);
			_distance = distance;
			UpdateCoords();
		}

		void SetDirection(glm::vec3 direction) {
			_direction = glm::normalize(direction);
			UpdateCoords();
		}

		void SetDistance(float distance) {
			_distance = distance;
			UpdateCoords();
		}
	};
}