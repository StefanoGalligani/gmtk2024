#pragma once

#include <sgEntity3D.h>

namespace sg {
	class View3D : public Entity3D {
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;

		void UpdateView() {
			_viewMatrix = glm::lookAt(GetLocalPosition(), GetLocalPosition() + Forward(), Up()); //TODO: usare la posizione globale
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

		void LookAt(glm::vec3 target, glm::vec3 up) {
			Entity3D::LookAt(target, up);
			UpdateView();
		}

		void LookAt(glm::vec3 target) {
			Entity3D::LookAt(target);
			UpdateView();
		}

		glm::mat4 GetViewProjection() {
			return _viewProjectionMatrix;
		}

		glm::mat4 GetView() {
			return _viewMatrix;
		}
	};
}