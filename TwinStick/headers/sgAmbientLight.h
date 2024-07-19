#pragma once

#include <sgEntity3D.h>

namespace sg {
	class AmbientLight : public sg::Entity3D {
	private:
		glm::vec3 _color;
		float _intensity;

	public:
		AmbientLight(glm::vec3 color = glm::vec3(1), float intensity = 1) : Entity3D() {
			_color = color;
			_intensity = intensity;
		}

		AmbientLight(float intensity) : AmbientLight(glm::vec3(1), intensity) {}

		void SetColor(glm::vec3 color) {
			_color = color;
		}

		glm::vec3 GetColor() {
			return _color;
		}

		void SetIntensity(float intensity) {
			_intensity = intensity;
		}

		float GetIntensity() {
			return _intensity;
		}
	};
}