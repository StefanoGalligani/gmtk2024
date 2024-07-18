#pragma once

#include <sgView3D.h>
#include <sgStructures.h>

namespace sg {
	class SpotLight3D : public View3D {
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
		SpotLight3D() : View3D() {
			_shadowMatrix = glm::mat4(1);
			_color = glm::vec3(1);
			_intensity = 1;
		}

		void SetupShadows(int width, int height, int textureUnit) {
			_depthBuffer = new sg::FrameBuffer(width, height, textureUnit, false, true, false, false);
			_shadowWidth = width;
			_shadowHeight = height;
		}

		void SetColor(glm::vec3 color) {
			_color = color;
		}

		glm::vec3 GetColor() {
			return _color;
		}

		void SetIntensity(float intensity) {
			_intensity = intensity;
		}

		float GetIntensity() {
			return _intensity;
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

		glm::mat4 GetShadow() {
			return _shadowMatrix;
		}

		~SpotLight3D() {
			delete(_depthBuffer);
		}
	};
}