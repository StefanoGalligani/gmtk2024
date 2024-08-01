#pragma once

#include <sgEntity3D.h>
#include <sgLight.h>

namespace sg {
	class PointLight3D : public Entity3D, public ShadowedLight3D {
	private:
		float _nearPlane;
		float _farPlane;
		float _range;
		Frustum _frustum;

		void UpdateProjectionMatrices() {

		}

		void UpdateViewMatrices() {
			UpdateFrustum();
		}

		void UpdateFrustum() {

		}

	protected:

		void LocalRotationFromGlobal(bool updateChildren = true) override { Entity3D::LocalRotationFromGlobal(updateChildren); UpdateViewMatrices(); }
		void LocalPositionFromGlobal(bool updateChildren = true) override { Entity3D::LocalPositionFromGlobal(updateChildren); UpdateViewMatrices(); }
		void GlobalRotationFromLocal(bool updateChildren = true) override { Entity3D::GlobalRotationFromLocal(updateChildren); UpdateViewMatrices(); }
		void GlobalPositionFromLocal(bool updateChildren = true) override { Entity3D::GlobalPositionFromLocal(updateChildren); UpdateViewMatrices(); }

	public:
		PointLight3D(int width, int height, float nearPlane, float farPlane) : Entity3D() {
			SetShadowWidth(width);
			SetShadowHeight(height);
			_nearPlane = nearPlane;
			_farPlane = farPlane;
			_range = farPlane;
			_lightType = TypePointLight;
			//inizializzare framebuffer
			UpdateViewMatrices();
			UpdateProjectionMatrices();
		}

		sg::Frustum GetFrustum() {
			return _frustum;
		}

		void SetNearPlane(float nearPlane) {
			_nearPlane = nearPlane;
			UpdateProjectionMatrices();
		}

		void SetFarPlane(float farPlane) {
			_farPlane = farPlane;
			UpdateProjectionMatrices();
		}

		void SetRange(float range) {
			_range = range;
		}

		float GetRange() {
			return _range;
		}
	};
}