#pragma once

#include <sgEntity3D.h>
#include <sgStructures.h>
#include <glm/glm/glm.hpp>

namespace sg {
	class View3D : public Entity3D {
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;
		float _fov;
		float _aspectRatio;
		float _nearPlane;
		float _farPlane;
		Frustum _frustum;
		bool _orthographic;

	protected:

		void UpdateFrustum()
		{
			const glm::vec3 frontMultFar = _farPlane * GlobalForward();
			_frustum.nearFace = { GetGlobalPosition() + _nearPlane * GlobalForward(), GlobalForward() };
			_frustum.farFace = { GetGlobalPosition() + frontMultFar, -GlobalForward() };

			if (!_orthographic) {
				const float halfVSide = _farPlane * tanf(_fov * .5f);
				const float halfHSide = halfVSide * _aspectRatio;

				_frustum.rightFace = { GetGlobalPosition(),
										glm::cross(frontMultFar - GlobalRight() * halfHSide, GlobalUp()) };
				_frustum.leftFace = { GetGlobalPosition(),
										glm::cross(GlobalUp(),frontMultFar + GlobalRight() * halfHSide) };
				_frustum.topFace = { GetGlobalPosition(),
										glm::cross(GlobalRight(), frontMultFar - GlobalUp() * halfVSide) };
				_frustum.bottomFace = { GetGlobalPosition(),
										glm::cross(frontMultFar + GlobalUp() * halfVSide, GlobalRight()) };
			}
			else
			{
				const float halfHSide = 2 * _fov;
				const float halfVSide = halfHSide * _aspectRatio;

				_frustum.rightFace = { GetGlobalPosition() + GlobalRight() * halfHSide, -GlobalRight() };
				_frustum.leftFace = { GetGlobalPosition() - GlobalRight() * halfHSide, GlobalRight() };
				_frustum.topFace = { GetGlobalPosition() + GlobalUp() * halfVSide, -GlobalUp() };
				_frustum.bottomFace = { GetGlobalPosition() - GlobalUp() * halfVSide, GlobalUp() };
			}
		}

		virtual void UpdateView() {
			_viewMatrix = glm::lookAt(GetGlobalPosition(), GetGlobalPosition() + GlobalForward(), GlobalUp());
			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
			UpdateFrustum();
		}

		void SetPerspective() {
			_orthographic = false;

			_projectionMatrix = glm::perspective(_fov, _aspectRatio, _nearPlane, _farPlane);
			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
			UpdateFrustum();
		}

		void SetOrthogonal() {
			_orthographic = true;

			_projectionMatrix = glm::scale(glm::vec3(0.5, 0.5 * _aspectRatio, -0.5)); //siamo sicuri che _aspectRatio vada moltiplicato alla y e non alla x?
			_projectionMatrix = glm::translate(_projectionMatrix, glm::vec3(0, 0, 2));
			_projectionMatrix = glm::scale(_projectionMatrix, glm::vec3(1 / _fov, 1 / _fov, 1 / (_farPlane - _nearPlane)));
			_projectionMatrix = glm::translate(_projectionMatrix, glm::vec3(0, 0, _nearPlane));

			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
			UpdateFrustum();
		}

		virtual void UpdateProjectionMatrix() {
		}

		void LocalRotationFromGlobal(bool updateChildren = true) override { Entity3D::LocalRotationFromGlobal(updateChildren); UpdateView(); }
		void LocalPositionFromGlobal(bool updateChildren = true) override { Entity3D::LocalPositionFromGlobal(updateChildren); UpdateView(); }
		void GlobalRotationFromLocal(bool updateChildren = true) override { Entity3D::GlobalRotationFromLocal(updateChildren); UpdateView(); }
		void GlobalPositionFromLocal(bool updateChildren = true) override { Entity3D::GlobalPositionFromLocal(updateChildren); UpdateView(); }

	public:
		View3D(float fov, float aspectRatio, float nearPlane, float farPlane) : Entity3D() {
			_fov = fov;
			_aspectRatio = aspectRatio;
			_nearPlane = nearPlane;
			_farPlane = farPlane;
		}

		sg::Frustum GetFrustum() {
			return _frustum;
		}

		void SetFov(float fov) {
			_fov = fov;
			UpdateProjectionMatrix();
		}

		void SetAspectRatio(float aspectRatio) {
			_aspectRatio = aspectRatio;
			UpdateProjectionMatrix();
		}

		void SetNearPlane(float nearPlane) {
			_nearPlane = nearPlane;
			UpdateProjectionMatrix();
		}

		void SetFarPlane(float farPlane) {
			_farPlane = farPlane;
			UpdateProjectionMatrix();
		}

		glm::mat4 GetViewProjection() {
			return _viewProjectionMatrix;
		}

		glm::mat4 GetView() {
			return _viewMatrix;
		}
	};
}