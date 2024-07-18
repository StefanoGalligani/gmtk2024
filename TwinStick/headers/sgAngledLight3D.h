#pragma once

#include <glm/glm/gtx/transform.hpp>
#include <sgView3D.h>
#include <sgStructures.h>

namespace sg {
	class AngledLight3D : public View3D {
	private:
		glm::mat4 _shadowMatrix;
		sg::FrameBuffer *_depthBuffer;
		glm::vec3 _color;
		float _intensity;
		int _shadowWidth;
		int _shadowHeight;

	protected:
		void UpdateView() override {
			View3D::UpdateView();
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * GetViewProjection();
		}

	public:
		AngledLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane) : View3D(fov, aspectRatio, nearPlane, farPlane) {
			_shadowMatrix = glm::mat4(1);
			_color = glm::vec3(1);
			_intensity = 1;
			_depthBuffer = new sg::FrameBuffer(width, height, false, true, false, false);
			_shadowWidth = width;
			_shadowHeight = height;
		}

		GLuint GetShadowTexture() {
			return _depthBuffer->depthMap;
		}

		void SetColor(glm::vec3 color) {
			_color = color;
		}

		glm::vec3 GetColor() const {
			return _color;
		}

		void SetIntensity(float intensity) {
			_intensity = intensity;
		}

		float GetIntensity() const {
			return _intensity;
		}

		sg::FrameBuffer GetShadowBuffer() const {
			return *_depthBuffer;
		}

		int GetShadowWidth() const {
			return _shadowWidth;
		}

		int GetShadowHeight() const {
			return _shadowHeight;
		}

		glm::mat4 GetShadow() const {
			return _shadowMatrix;
		}

		~AngledLight3D() {
			delete(_depthBuffer);
		}
	};
}