#pragma once

#include <sgView3D.h>
#include <sgStructures.h>

namespace sg {
	class SpotLight3D : public View3D {
	private:
		glm::mat4 _shadowMatrix;
		float _bias;
		sg::FrameBuffer *_depthBuffer;
		int _shadowWidth;
		int _shadowHeight;

		void UpdateBias() {
			_bias = (1.0f / (glm::length2(GetLocalPosition()) + 1)) * 0.3;
		}

	public:
		SpotLight3D() : View3D() {
			_shadowMatrix = glm::mat4(1);
		}

		void LookAt(glm::vec3 target) {
			View3D::LookAt(target);
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - _bias)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * GetViewProjection();
		}

		void SetupShadows(int width, int height, int textureUnit) {
			_depthBuffer = new sg::FrameBuffer(width, height, textureUnit, false, true, false, false);
			_shadowWidth = width;
			_shadowHeight = height;
		}

		void Translate(float x, float y, float z) { View3D::Translate(x, y, z); UpdateBias(); }
		void Translate(glm::vec3 vec) { View3D::Translate(vec); }
		void SetLocalPosition(float x, float y, float z) { View3D::SetLocalPosition(x, y, z); UpdateBias(); }
		void SetLocalPosition(glm::vec3 pos) { View3D::SetLocalPosition(pos); UpdateBias(); }

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