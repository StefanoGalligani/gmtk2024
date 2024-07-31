#pragma once

#include <sgAngledLight3D.h>

namespace sg {
	class SpotLight3D : public AngledLight3D {
	private:
		bool _orthographic;
		float _range;

	protected:
		void UpdateProjectionMatrix() override {
			if (_orthographic) SetOrthogonal();
			else SetPerspective();
		}

	public:
		SpotLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane, bool orthogonal = false)
			: AngledLight3D(width, height, fov, aspectRatio, nearPlane, farPlane) {
			_orthographic = orthogonal;
			_lightType = TypeSpotLight;
			_range = farPlane;
			UpdateProjectionMatrix();
		}

		void SetRange(float range) {
			_range = range;
		}

		float GetRange() {
			return _range;
		}
	};
}