#pragma once

#include <sgAngledLight3D.h>

namespace sg {
	class SpotLight3D : public AngledLight3D {
	private:
		bool _orthogonal;

	protected:
		void UpdateProjectionMatrix() override {
			if (_orthogonal) SetOrthogonal();
			else SetPerspective();
		}

	public:
		SpotLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane, bool orthogonal = false)
			: AngledLight3D(width, height, fov, aspectRatio, nearPlane, farPlane) {
			_orthogonal = orthogonal;
			UpdateProjectionMatrix();
		}
	};
}