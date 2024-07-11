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
			_bias = 0.0001f;
		}

	protected:
		void UpdateView() override {
			View3D::UpdateView();
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - _bias)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * GetViewProjection();
			UpdateBias();
		}

	public:
		SpotLight3D() : View3D() {
			_shadowMatrix = glm::mat4(1);
		}

		void LookAtLocal(glm::vec3 target) override {
			View3D::LookAtLocal(target);
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - _bias)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * GetViewProjection();
		}

		void LookAtGlobal(glm::vec3 target) override {
			View3D::LookAtGlobal(target);
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f - _bias)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * GetViewProjection();
		}

		void SetupShadows(int width, int height, int textureUnit) {
			_depthBuffer = new sg::FrameBuffer(width, height, textureUnit, false, true, false, false);
			_shadowWidth = width;
			_shadowHeight = height;
		}

		/*void TranslateGlobal(float x, float y, float z) override { View3D::TranslateGlobal(x, y, z); UpdateBias(); }
		void TranslateGlobal(glm::vec3 vec) override { View3D::TranslateGlobal(vec); UpdateBias(); }
		void TranslateLocal(float x, float y, float z) override { View3D::TranslateLocal(x, y, z); UpdateBias(); }
		void TranslateLocal(glm::vec3 vec) override { View3D::TranslateLocal(vec); UpdateBias(); }
		void SetLocalPosition(float x, float y, float z) override { View3D::SetLocalPosition(x, y, z); UpdateBias(); }
		void SetLocalPosition(glm::vec3 pos) override { View3D::SetLocalPosition(pos); UpdateBias(); }
		void SetGlobalPosition(float x, float y, float z) override { View3D::SetGlobalPosition(x, y, z); UpdateBias(); }
		void SetGlobalPosition(glm::vec3 pos) override { View3D::SetGlobalPosition(pos); UpdateBias(); }*/

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