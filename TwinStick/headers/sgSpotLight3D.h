#pragma once

#include <sgAngledLight3D.h>

namespace sg {
	class SpotLight3D : public AngledLight3D {
	private:
		bool _orthogonal;
		float _range;

	protected:
		void UpdateProjectionMatrix() override {
			if (_orthogonal) SetOrthogonal();
			else SetPerspective();
		}

	public:
		SpotLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane, bool orthogonal = false)
			: AngledLight3D(width, height, fov, aspectRatio, nearPlane, farPlane) {
			_orthogonal = orthogonal;
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