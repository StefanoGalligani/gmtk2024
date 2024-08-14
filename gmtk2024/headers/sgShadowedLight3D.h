#pragma once

#include <sgLight.h>

namespace sg {
	class ShadowedLight3D : public Light {
	private:
		int _shadowWidth;
		int _shadowHeight;

	protected:
		void SetShadowWidth(int width) {
			_shadowWidth = width;
		}

		void SetShadowHeight(int height) {
			_shadowHeight = height;
		}

	public:
		int GetShadowWidth() const {
			return _shadowWidth;
		}

		int GetShadowHeight() const {
			return _shadowHeight;
		}
	};
}