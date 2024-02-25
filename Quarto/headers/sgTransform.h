#pragma once
#include <glm/glm/glm.hpp>

/*
TODO:
	metodi per rotazione
*/

namespace sg {
	struct Transform {
	public:
		glm::vec3 localPosition;
		glm::vec3 localScale;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		Transform() {
			localPosition = glm::vec3(0);
			localScale = glm::vec3(1, 1, 1);
			forward = glm::vec3(0, 0, -1);
			right = glm::vec3(1, 0, 0);
			up = glm::vec3(0, 1, 0);
		}

		void Translate(float x, float y, float z);
		void Translate(glm::vec3 vec);

		void ResetRotation();
		void Rotate(float x, float y, float z);
		void Rotate(glm::vec3 vec, float angle);
		void RotateAround(glm::vec3 axis, glm::vec3 point, float angle);

		void Scale(float x, float y, float z);
		void Scale(glm::vec3 scale);
	};
	#pragma region Position

	inline void Transform::Translate(float x, float y, float z) {
		localPosition.x += x;
		localPosition.y += y;
		localPosition.z += z;
	}

	inline void Transform::Translate(glm::vec3 vec) {
		localPosition += vec;
	}

	#pragma endregion

	#pragma region Rotation

		inline void Transform::ResetRotation() {
			forward = glm::vec3(0, 0, -1);
			right = glm::vec3(1, 0, 0);
			up = glm::vec3(0, 1, 0);
		}

		inline void Transform::Rotate(float x, float y, float z) {
			Rotate(glm::vec3(1, 0, 0), x);
			Rotate(glm::vec3(0, 1, 0), y);
			Rotate(glm::vec3(0, 0, 1), z);
		}

		inline void Transform::Rotate(glm::vec3 axis, float angle) {
			float axisComponent = glm::dot(axis, forward);
			float newXComponent = glm::sqrt((glm::length2(forward) - axisComponent * axisComponent));
			glm::vec3 newY = glm::cross(axis, forward);
			if (glm::length2(newY) > 0.0001f) {
				newY = glm::normalize(newY);
				glm::vec3 newX = glm::cross(newY, axis);
				forward = (newX * cos(angle) + newY * sin(angle)) * newXComponent + axis * axisComponent;
			}

			axisComponent = glm::dot(axis, up);
			newXComponent = glm::sqrt((glm::length2(up) - axisComponent * axisComponent));
			newY = glm::cross(axis, up);
			if (glm::length2(newY) > 0.0001f) {
				newY = glm::normalize(newY);
				glm::vec3 newX = glm::cross(newY, axis);
				up = (newX * cos(angle) + newY * sin(angle)) * newXComponent + axis * axisComponent;
			}

			forward = glm::normalize(forward);
			up = glm::normalize(up);

			right = glm::normalize(glm::cross(forward, up));
		}

		inline void Transform::RotateAround(glm::vec3 axis, glm::vec3 point, float angle) {
			axis = glm::normalize(axis);
			glm::vec3 temp = localPosition - point;
			float axisComponent = glm::dot(axis, temp);
			float newXComponent = glm::sqrt((glm::length2(temp) - axisComponent * axisComponent));
			glm::vec3 newY = glm::cross(axis, temp);
			if (glm::length2(newY) > 0.0001f) {
				newY = glm::normalize(newY);
				glm::vec3 newX = glm::cross(newY, axis);
				localPosition = (newX * cos(angle) + newY * sin(angle)) * newXComponent + axis * axisComponent;
			}
			localPosition  = localPosition + point;
			Rotate(axis, angle);
		}

	#pragma endregion

	#pragma region Scale

		inline void Transform::Scale(float x, float y, float z) {
			localScale.x += x;
			localScale.y += y;
			localScale.z += z;
		}

		inline void Transform::Scale(glm::vec3 scale) {
			localScale += scale;
		}

	#pragma endregion
}