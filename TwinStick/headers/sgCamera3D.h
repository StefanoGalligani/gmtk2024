#pragma once

#include <sgView3D.h>

namespace sg {
	class Camera3D : public View3D {
	private:

	protected:
		void UpdateProjectionMatrix() override {
			SetPerspective();
		}

	public:
		Camera3D(float fov, float aspectRatio, float nearPlane, float farPlane) : View3D(fov, aspectRatio, nearPlane, farPlane) {
			UpdateProjectionMatrix();
		}
	};
}