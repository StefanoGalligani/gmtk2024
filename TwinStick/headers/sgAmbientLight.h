#pragma once

#include <sgEntity3D.h>
#include <sgLight.h>

namespace sg {
	class AmbientLight : public Entity3D, public sg::Light {
	public:
		AmbientLight(glm::vec3 color = glm::vec3(1), float intensity = 1) : Light() {
			SetColor(color);
			SetIntensity(intensity);
			_lightType = TypeAmbientLight;
		}

		AmbientLight(float intensity) : AmbientLight(glm::vec3(1), intensity) {}
	};
}