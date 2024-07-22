#pragma once

#define TypeAmbientLight 1
#define TypeDirectionalLight 2
#define TypeSpotLight 3
#define TypePointLight 4

namespace sg {
	typedef unsigned int LightType;

	class Light {
	private:
		glm::vec3 _color = glm::vec3(1);
		float _intensity = 1;

	protected:
		LightType _lightType;

	public:
		LightType GetLightType() { return _lightType; }

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