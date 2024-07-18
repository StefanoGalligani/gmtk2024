#pragma once

#include <sgEntity3D.h>

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

	protected:
		virtual void UpdateView() {
			_viewMatrix = glm::lookAt(GetGlobalPosition(), GetGlobalPosition() + GlobalForward(), GlobalUp());
			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
		}

		void SetPerspective() {
			_projectionMatrix = glm::perspective(_fov, _aspectRatio, _nearPlane, _farPlane);

			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
		}

		void SetOrthogonal() {
			_projectionMatrix = glm::scale(glm::vec3(0.5, 0.5 * _aspectRatio, -0.5));
			_projectionMatrix = glm::translate(_projectionMatrix, glm::vec3(0, 0, 2));
			_projectionMatrix = glm::scale(_projectionMatrix, glm::vec3(1 / _fov, 1 / _fov, 1 / (_farPlane - _nearPlane)));
			_projectionMatrix = glm::translate(_projectionMatrix, glm::vec3(0, 0, _nearPlane));

			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
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