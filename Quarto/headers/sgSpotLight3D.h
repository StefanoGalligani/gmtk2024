#pragma once

#include <sgTransform.h>
#include <sgStructures.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/vector_angle.hpp>

namespace sg {
	class SpotLight3D {
	private:
		Transform _transform;
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjectionMatrix;
		glm::mat4 _shadowMatrix;
		float _bias;
		sg::FrameBuffer *_depthBuffer;
		int _shadowWidth;
		int _shadowHeight;

	public:
		SpotLight3D() {
			_transform = Transform();
			_projectionMatrix = glm::mat4(1);
			_viewMatrix = glm::mat4(1);
			_viewProjectionMatrix = glm::mat4(1);
			_shadowMatrix = glm::mat4(1);
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

		void LookAt(glm::vec3 target) {
			_viewMatrix = glm::lookAt(_transform.localPosition, target, glm::vec3(0, 1, 0));
			_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
			//TODO: in futuro dovrà prendere la posizione globale della luce, non quella locale
			_bias = (1.0f / (glm::length2(_transform.localPosition) + 1)) * 0.3;
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - _bias)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * _viewProjectionMatrix;
		}

		void SetupShadows(int width, int height, int textureUnit) {
			_depthBuffer = (FrameBuffer*)malloc(sizeof(FrameBuffer));
			*_depthBuffer = sg::FrameBuffer(width, height, textureUnit, false, true, false, false);
			_shadowWidth = width;
			_shadowHeight = height;
		}

		sg::FrameBuffer GetShadowBuffer() {
			return *_depthBuffer;
		}

		int GetShadowWidth() {
			return _shadowWidth;
		}

		int GetShadowHeight() {
			return _shadowHeight;
		}

		glm::mat4 GetViewProjection() {
			return _viewProjectionMatrix;
		}

		glm::mat4 GetView() {
			return _viewMatrix;
		}

		glm::mat4 GetShadow() {
			return _shadowMatrix;
		}

		void Translate(float x, float y, float z) { _transform.Translate(x, y, z); }
		void Translate(glm::vec3 vec) { _transform.Translate(vec); }
		void SetLocalPosition(float x, float y, float z) { _transform.localPosition = glm::vec3(x, y, z); }
		void SetLocalPosition(glm::vec3 pos) { _transform.localPosition = pos; }
		glm::vec3 GetLocalPosition() { return _transform.localPosition; }

		void Rotate(float x, float y, float z) { _transform.Rotate(x, y, z); }
		void Rotate(glm::vec3 axis, float angle) { _transform.Rotate(axis, angle); }
		void RotateAround(glm::vec3 axis, glm::vec3 point, float angle) { _transform.RotateAround(axis, point, angle); }
		void SetLocalRotation(float x, float y, float z) { _transform.ResetRotation();  _transform.Rotate(x, y, z); }
		void ResetRotation() { _transform.ResetRotation(); }

		void Scale(float x, float y, float z) { _transform.Scale(x, y, z); }
		void Scale(glm::vec3 scale) { _transform.Scale(scale); }
		void SetLocalScale(float x, float y, float z) { _transform.localScale = glm::vec3(x, y, z); }
		void SetLocalScale(glm::vec3 scale) { _transform.localScale = scale; }
		void SetLocalUniformScale(float s) { _transform.localScale = glm::vec3(s, s, s); }
	};
}