#pragma once

#include <sgEntity3D.h>

namespace sg {
	class View3D : public Entity3D {
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;

		void UpdateView() {
			_viewMatrix = glm::lookAt(GetGlobalPosition(), GetGlobalPosition() + GlobalForward(), GlobalUp());
			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
		}

	public:
		View3D() : Entity3D() {
			_projectionMatrix = glm::mat4(1);
			_viewMatrix = glm::mat4(1);
			_viewProjectionMatrix = glm::mat4(1);
		}

		void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
			_projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
		}

		void SetOrthogonal(float aspectRatio, float zoom) {
			_projectionMatrix = glm::scale(glm::vec3(0.5, 0.5 * aspectRatio, -0.05));
			_projectionMatrix = glm::translate(_projectionMatrix, glm::vec3(0, 0, 1));
			_projectionMatrix = glm::scale(_projectionMatrix, glm::vec3(zoom, zoom, zoom));

			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
		}

		//TODO ereditare metodi locali e aggiornare la vista
		void LookAtLocal(glm::vec3 target, glm::vec3 up) { Entity3D::LookAtLocal(target, up); UpdateView(); }
		void LookAtLocal(glm::vec3 target) { Entity3D::LookAtLocal(target); UpdateView(); }

		void TranslateGlobal(float x, float y, float z) { Entity3D::TranslateGlobal(x, y, z); UpdateView(); }
		void TranslateGlobal(glm::vec3 vec) { Entity3D::TranslateGlobal(vec); UpdateView(); }
		void SetGlobalPosition(float x, float y, float z) { Entity3D::SetGlobalPosition(x, y, z); UpdateView(); }
		void SetGlobalPosition(glm::vec3 pos) { Entity3D::SetGlobalPosition(pos); UpdateView(); }
		void RotateGlobal(float x, float y, float z) { Entity3D::RotateGlobal(x, y, z); UpdateView(); }
		void RotateGlobal(glm::vec3 axis, float angle) { Entity3D::RotateGlobal(axis, angle); UpdateView(); }
		void RotateAroundGlobal(glm::vec3 axis, glm::vec3 point, float angle) { Entity3D::RotateAroundGlobal(axis, point, angle); UpdateView(); }
		void LookAtGlobal(glm::vec3 target, glm::vec3 up) { Entity3D::LookAtGlobal(target, up); UpdateView(); }
		void LookAtGlobal(glm::vec3 target) { Entity3D::LookAtGlobal(target); UpdateView(); }

		glm::mat4 GetViewProjection() {
			return _viewProjectionMatrix;
		}

		glm::mat4 GetView() {
			return _viewMatrix;
		}
	};
}