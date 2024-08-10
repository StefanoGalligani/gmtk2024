#pragma once

#define TypeAmbientLight 1
#define TypeDirectionalLight 2
#define TypeSpotLight 3
#define TypePointLight 4

namespace sg {
	typedef unsigned int LightType;

	class Light {
	private:
		glm::vec3 _color = glm::vec3(1);
		float _intensity = 1;
		glm::vec3 _center;
		glm::vec3 _extents;
		bool _boundingBoxSet = false;

	protected:
		LightType _lightType;

		void SetBoundingBox(glm::vec3 center, glm::vec3 extents) {
			_center = center;
			_extents = extents;
			_boundingBoxSet = true;
		}

	public:
		LightType GetLightType() { return _lightType; }

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

		bool FrustumCheck(sg::Frustum frustum) {
			if (!_boundingBoxSet) return true;
			return (isOnOrForwardPlane(frustum.leftFace, _center, _extents) &&
				isOnOrForwardPlane(frustum.rightFace, _center, _extents) &&
				isOnOrForwardPlane(frustum.topFace, _center, _extents) &&
				isOnOrForwardPlane(frustum.bottomFace, _center, _extents) &&
				isOnOrForwardPlane(frustum.nearFace, _center, _extents) &&
				isOnOrForwardPlane(frustum.farFace, _center, _extents));
		}

		bool isOnOrForwardPlane(const Plane& plane, glm::vec3 center, glm::vec3 extents) const
		{
			const float r = glm::dot(extents, glm::abs(plane.normal));
			return -r <= glm::dot(plane.normal, center) - plane.distance;
		}
	};
}