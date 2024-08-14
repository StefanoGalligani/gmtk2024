#pragma once

#include <glm/glm/gtx/transform.hpp>
#include <sgView3D.h>
#include <sgStructures.h>
#include <sgShadowedLight3D.h>

namespace sg {
	class AngledLight3D : public View3D, public ShadowedLight3D {
	private:
		glm::mat4 _shadowMatrix;
		sg::FrameBuffer *_depthBuffer;

		void SetBoundingBox() {
			const float farPlane = GetFarPlane();
			const float nearPlane = GetNearPlane();
			const float fov = GetFov();
			const float aspectRatio = GetAspectRatio();

			glm::vec3 center = GetGlobalPosition() + GlobalForward() * (farPlane + nearPlane) / 2.0f;
			glm::vec3 localExtents;
			localExtents.z = (farPlane - nearPlane) / 2.0f;

			const float halfVSide = IsOrthographic() ? 2 * fov : farPlane * tanf(fov * .5f);
			const float halfHSide = halfVSide * aspectRatio;

			localExtents.x = halfHSide;
			localExtents.y = halfVSide;

			Light::SetBoundingBox(center, LocalToGlobalExtents(localExtents));
		}

		glm::vec3 LocalToGlobalExtents(glm::vec3 localExtents) {
			const glm::vec3 right = LocalRight() * localExtents.x;
			const glm::vec3 up = LocalUp() * localExtents.y;
			const glm::vec3 forward = LocalForward() * localExtents.z;

			const float newIi = std::abs(right.x) + std::abs(up.x) + std::abs(forward.x);
			const float newIj = std::abs(right.y) + std::abs(up.y) + std::abs(forward.y);
			const float newIk = std::abs(right.z) + std::abs(up.z) + std::abs(forward.z);

			return { newIi, newIj, newIk };
		}

	protected:
		void UpdateView() override {
			View3D::UpdateView();

			glm::mat4 viewProj = GetProjection() * glm::translate(glm::vec3(0, 0, 0.05f)) * GetView();
			_shadowMatrix = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * viewProj;
			SetBoundingBox();
		}

	public:
		AngledLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane) : View3D(fov, aspectRatio, nearPlane, farPlane) {
			_shadowMatrix = glm::mat4(1);
			_depthBuffer = new sg::FrameBuffer(width, height, false, true, false, false);
			SetShadowWidth(width);
			SetShadowHeight(height);
		}

		GLuint GetShadowTexture() {
			return _depthBuffer->depthMap;
		}

		sg::FrameBuffer GetShadowBuffer() const {
			return *_depthBuffer;
		}

		glm::mat4 GetShadow() const {
			return _shadowMatrix;
		}

		~AngledLight3D() {
			_depthBuffer->FreeTextures();
			delete(_depthBuffer);
		}
	};
}