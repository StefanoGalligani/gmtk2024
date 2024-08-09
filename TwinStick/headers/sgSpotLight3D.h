#pragma once

#include <sgAngledLight3D.h>

namespace sg {
	class SpotLight3D : public AngledLight3D {
	private:
		bool _orthographic;
		float _range;
		sg::Texture _mapTexture;

	protected:
		void UpdateProjectionMatrix() override {
			if (_orthographic) SetOrthogonal();
			else SetPerspective();
		}

	public:
		SpotLight3D(int width, int height, float fov, float aspectRatio, float nearPlane, float farPlane, bool orthographic = false)
			: AngledLight3D(width, height, fov, aspectRatio, nearPlane, farPlane) {
			_orthographic = orthographic;
			_lightType = TypeSpotLight;
			_range = farPlane;
			UpdateProjectionMatrix();
		}

		void SetMapTexture(const char* filename) {
			_mapTexture.map = (char*)filename;
			_mapTexture.index = sg::TextureManager::Instance()->SetTexture(filename);
			_mapTexture.isPresent = true;
			_mapTexture.isLoaded = true;
		}

		sg::Texture GetMapTexture() {
			return _mapTexture;
		}

		void SetRange(float range) {
			_range = range;
		}

		float GetRange() {
			return _range;
		}
	};
}