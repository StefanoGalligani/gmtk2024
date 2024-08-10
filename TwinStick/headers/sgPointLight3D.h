#pragma once

#include <sgEntity3D.h>
#include <sgLight.h>
#include <sgStructures.h>

#define FOV 3.14159265358979323846f/2

namespace sg {
	class PointLight3D : public Entity3D, public ShadowedLight3D {
	private:
		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrices[6];
		glm::mat4 _viewProjectionMatrices[6];
		std::vector<glm::vec3> _directions = { glm::vec3(1,0,0), glm::vec3(-1,0,0), glm::vec3(0,1,0), glm::vec3(0,-1,0), glm::vec3(0, 0, 1), glm::vec3(0, 0, -1) };
		std::vector<glm::vec3> _right = { glm::vec3(0,0,-1), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(1,0,0), glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0)};
		std::vector<glm::vec3> _up = { glm::vec3(0,-1,0), glm::vec3(0,-1,0), glm::vec3(0,0,1), glm::vec3(0,0,-1), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0) };
		float _nearPlane;
		float _farPlane;
		float _range;
		Frustum _frustums[6];
		FrameBufferCube *_depthCubeBuffer;

		void UpdateProjectionMatrix() {
			_projectionMatrix = glm::perspective(FOV, 1.0f, _nearPlane, _farPlane);
		}

		void UpdateViewMatrices() {
			for (int i = 0; i < 6; i++) {
				_viewMatrices[i] = glm::lookAt(GetGlobalPosition(), GetGlobalPosition() + _directions[i], _up[i]);
				_viewProjectionMatrices[i] = _projectionMatrix * _viewMatrices[i];
			}
			UpdateFrustums();
		}

		void UpdateFrustums() {
			const float halfSide = _farPlane * tanf(FOV * .5f);

			for (int i=0; i<6; i++) {
				const glm::vec3 frontMultFar = _farPlane * _directions[i];
				_frustums[i].nearFace = {GetGlobalPosition() + _nearPlane * _directions[i], _directions[i] };
				_frustums[i].farFace = {GetGlobalPosition() + frontMultFar, -_directions[i]};
				_frustums[i].rightFace = {GetGlobalPosition(),
										glm::cross(frontMultFar - _right[i] * halfSide, _up[i])};
				_frustums[i].leftFace = {GetGlobalPosition(),
										glm::cross(_up[i],frontMultFar + _right[i] * halfSide) };
				_frustums[i].topFace = {GetGlobalPosition(),
										glm::cross(_right[i], frontMultFar - _up[i] * halfSide) };
				_frustums[i].bottomFace = {GetGlobalPosition(),
										glm::cross(frontMultFar + _up[i] * halfSide, _right[i]) };
			}
		}

		void UpdateBoundingBox() {
			SetBoundingBox(GetGlobalPosition(), glm::vec3(_farPlane, _farPlane, _farPlane) / 2.0f);
		}

	protected:

		void LocalPositionFromGlobal(bool updateChildren = true) override { Entity3D::LocalPositionFromGlobal(updateChildren); UpdateViewMatrices(); UpdateBoundingBox(); }
		void GlobalPositionFromLocal(bool updateChildren = true) override { Entity3D::GlobalPositionFromLocal(updateChildren); UpdateViewMatrices(); UpdateBoundingBox(); }

	public:
		PointLight3D(int resolution, float nearPlane, float farPlane) : Entity3D() {
			SetShadowWidth(resolution);
			SetShadowHeight(resolution);
			_nearPlane = nearPlane;
			_farPlane = farPlane;
			_range = farPlane;
			_lightType = TypePointLight;
			_depthCubeBuffer = new sg::FrameBufferCube(resolution, false, true, false);
			UpdateProjectionMatrix();
			UpdateViewMatrices();
		}

		GLuint GetShadowTexture() {
			return _depthCubeBuffer->depthMap;
		}

		sg::FrameBufferCube GetShadowBuffer() const {
			return *_depthCubeBuffer;
		}

		sg::Frustum GetFrustum(int index) {
			return _frustums[index];
		}

		void SetNearPlane(float nearPlane) {
			_nearPlane = nearPlane;
			UpdateProjectionMatrix();
		}

		void SetFarPlane(float farPlane) {
			_farPlane = farPlane;
			UpdateProjectionMatrix();
			UpdateBoundingBox();
		}

		float GetNearPlane() {
			return _nearPlane;
		}

		float GetFarPlane() {
			return _farPlane;
		}

		void SetRange(float range) {
			_range = range;
		}

		float GetRange() {
			return _range;
		}

		glm::mat4 GetViewProjection(int index) {
			return _viewProjectionMatrices[index];
		}

		glm::mat4 GetView(int index) {
			return _viewMatrices[index];
		}

		~PointLight3D() {
			_depthCubeBuffer->FreeTextures();
			delete(_depthCubeBuffer);
		}
	};
}