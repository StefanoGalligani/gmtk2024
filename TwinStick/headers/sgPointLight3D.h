#pragma once

#include <sgEntity3D.h>
#include <sgLight.h>

namespace sg {
	class PointLight3D : public Entity3D, public Light {
	private:
	public:
		PointLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane) {

		}
	};
}