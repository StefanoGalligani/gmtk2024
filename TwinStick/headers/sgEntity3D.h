#pragma once
#include <sgTransform.h>
#include <list>

namespace sg {
	class Entity3D {
	private:
		static unsigned int nextId;
		int _id;
		std::list<Entity3D*> _children;

	protected:
		Transform _transform;

	public:

		Entity3D() : _id(nextId++) {
			_transform = Transform();
			_children = std::list<Entity3D*>();
		}

		void Translate(float x, float y, float z) { _transform.Translate(x, y, z); }
		void Translate(glm::vec3 vec) { _transform.Translate(vec); }
		void SetLocalPosition(float x, float y, float z) { _transform.localPosition = glm::vec3(x, y, z); }
		void SetLocalPosition(glm::vec3 pos) { _transform.localPosition = pos; }
		glm::vec3 GetLocalPosition() { return _transform.localPosition; }

		void Rotate(float x, float y, float z) { _transform.Rotate(x, y, z); }
		void Rotate(glm::vec3 axis, float angle) { _transform.Rotate(axis, angle); }
		void RotateAround(glm::vec3 axis, glm::vec3 point, float angle) { _transform.RotateAround(axis, point, angle); }
		void SetLocalRotation(float x, float y, float z) { _transform.ResetRotation();  _transform.Rotate(x, y, z); }
		void ResetRotation() { _transform.ResetRotation(); }

		void Scale(float x, float y, float z) { _transform.Scale(x, y, z); }
		void Scale(glm::vec3 scale) { _transform.Scale(scale); }
		void SetLocalScale(float x, float y, float z) { _transform.localScale = glm::vec3(x, y, z); }
		void SetLocalScale(glm::vec3 scale) { _transform.localScale = scale; }
		void SetLocalUniformScale(float s) { _transform.localScale = glm::vec3(s, s, s); }

		glm::vec3 Forward() { return _transform.forward; }
		glm::vec3 Up() { return _transform.up; }
		glm::vec3 Right() { return _transform.right; }

		void AddChild(Entity3D* child) {
			_children.push_back(child);
		}

		void RemoveChild(Entity3D* child) {
			_children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
		}
	};
	unsigned int Entity3D::nextId;
}