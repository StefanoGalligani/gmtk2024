#pragma once
#include <sgTransform.h>
#include <list>

namespace sg {
	class Entity3D {
	protected:
		Transform _localTransform;
		Transform _globalTransform;
		Entity3D* _parent;
		std::list<Entity3D*> _children;

	private:
		static unsigned int nextId;
		int _id;

		void LocalTransformFromGlobal() {
			LocalRotationFromGlobal(false);
			LocalScaleFromGlobal(false);
			LocalPositionFromGlobal(false);
			UpdateGlobalTransformInChildren();
		}

		void GlobalTransformFromLocal() {
			GlobalRotationFromLocal(false);
			GlobalScaleFromLocal(false);
			GlobalPositionFromLocal(false);
			UpdateGlobalTransformInChildren();
		}

		glm::mat3 LocalRotationToGlobalMatrix() {
			glm::mat3 matrix = glm::mat3();
			matrix[0][0] = _parent->_globalTransform.right.x;
			matrix[1][0] = _parent->_globalTransform.right.y;
			matrix[2][0] = -_parent->_globalTransform.right.z;
			matrix[0][1] = _parent->_globalTransform.up.x;
			matrix[1][1] = _parent->_globalTransform.up.y;
			matrix[2][1] = -_parent->_globalTransform.up.z;
			matrix[0][2] = _parent->_globalTransform.forward.x;
			matrix[1][2] = _parent->_globalTransform.forward.y;
			matrix[2][2] = -_parent->_globalTransform.forward.z;
			return matrix;
		}

		glm::mat3 LocalPositionToGlobalMatrix() {
			glm::mat3 matrix = glm::mat3();
			float x = _globalTransform.scale.x * glm::abs(_parent->_globalTransform.right.x) +
					_globalTransform.scale.y * glm::abs(_parent->_globalTransform.up.x) +
					_globalTransform.scale.z * glm::abs(_parent->_globalTransform.forward.x);
			float y = _globalTransform.scale.x * glm::abs(_parent->_globalTransform.right.y) +
				_globalTransform.scale.y * glm::abs(_parent->_globalTransform.up.y) +
				_globalTransform.scale.z * glm::abs(_parent->_globalTransform.forward.y);
			float z = _globalTransform.scale.x * glm::abs(_parent->_globalTransform.right.z) +
				_globalTransform.scale.y * glm::abs(_parent->_globalTransform.up.z) +
				_globalTransform.scale.z * glm::abs(_parent->_globalTransform.forward.z);
			//TODO non proprio corretto, le rotazioni fanno movimenti strani

			matrix[0][0] = x * _parent->GlobalRight().x;
			matrix[1][0] = x * _parent->GlobalRight().y;
			matrix[2][0] = x * _parent->GlobalRight().z;
			matrix[0][1] = y * _parent->GlobalUp().x;
			matrix[1][1] = y * _parent->GlobalUp().y;
			matrix[2][1] = y * _parent->GlobalUp().z;
			matrix[0][2] = z * _parent->GlobalForward().x;
			matrix[1][2] = z * _parent->GlobalForward().y;
			matrix[2][2] = z * _parent->GlobalForward().z;
			return matrix;
		}

	protected:

		virtual void LocalRotationFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.forward = _globalTransform.forward;
				_localTransform.right = _globalTransform.right;
				_localTransform.up = _globalTransform.up;
			} else {
				glm::mat3 toGlob = LocalRotationToGlobalMatrix();
				glm::mat3 toLoc = glm::inverse(toGlob);
				_localTransform.forward = toLoc * GlobalForward();
				_localTransform.right = toLoc * GlobalRight();
				_localTransform.up = toLoc * GlobalUp();
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		virtual void LocalScaleFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.scale = _globalTransform.scale;
			} else {
				_localTransform.scale = _globalTransform.scale / _parent->_globalTransform.scale;
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		virtual void LocalPositionFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.position = _globalTransform.position;
			} else {
				glm::mat3 toGlob = LocalPositionToGlobalMatrix();
				_localTransform.position = glm::inverse(toGlob) * (GetGlobalPosition() - _parent->GetGlobalPosition());
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		virtual void GlobalRotationFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.forward = LocalForward();
				_globalTransform.right = LocalRight();
				_globalTransform.up = LocalUp();
			} else {
				glm::mat3 toGlob = LocalRotationToGlobalMatrix();
				_globalTransform.forward = toGlob * LocalForward();
				_globalTransform.right = toGlob * LocalRight();
				_globalTransform.up = toGlob * LocalUp();
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		virtual void GlobalScaleFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.scale = _localTransform.scale;
			} else {
				_globalTransform.scale = _localTransform.scale * _parent->_globalTransform.scale;
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		virtual void GlobalPositionFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.position = _localTransform.position;
			} else {
				glm::mat3 toGlob = LocalPositionToGlobalMatrix();
				_globalTransform.position = _parent->GetGlobalPosition() + toGlob * GetLocalPosition();
			}

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void UpdateGlobalTransformInChildren() {
			for (auto const& child : _children) {
				child->GlobalTransformFromLocal();
			}
		}

	public:

		Entity3D() : _id(nextId++) {
			_parent = NULL;
		}

		#pragma region Local

		virtual void TranslateLocal(float x, float y, float z) { _localTransform.Translate(x, y, z); GlobalPositionFromLocal(); }
		virtual void TranslateLocal(glm::vec3 vec) { _localTransform.Translate(vec); GlobalPositionFromLocal(); }
		virtual void SetLocalPosition(float x, float y, float z) { _localTransform.position = glm::vec3(x, y, z); GlobalPositionFromLocal(); }
		virtual void SetLocalPosition(glm::vec3 pos) { _localTransform.position = pos; GlobalPositionFromLocal(); }
		glm::vec3 GetLocalPosition() { return _localTransform.position; }

		virtual void RotateLocal(float x, float y, float z) { _localTransform.Rotate(x, y, z); GlobalRotationFromLocal(); }
		virtual void RotateLocal(glm::vec3 axis, float angle) { _localTransform.Rotate(axis, angle); GlobalRotationFromLocal(); }
		virtual void RotateAroundLocal(glm::vec3 axis, glm::vec3 point, float angle) {
			_localTransform.RotateAround(axis, point, angle);
			GlobalRotationFromLocal();
			GlobalPositionFromLocal();
		}
		virtual void SetLocalRotation(float x, float y, float z) { _localTransform.ResetRotation();  _localTransform.Rotate(x, y, z); GlobalRotationFromLocal(); }
		virtual void ResetLocalRotation() { _localTransform.ResetRotation(); GlobalRotationFromLocal(); }

		virtual void LookAtLocal(glm::vec3 target, glm::vec3 up) { _localTransform.LookAt(target, up); GlobalRotationFromLocal(); }
		virtual void LookAtLocal(glm::vec3 target) {
			glm::vec3 up = glm::vec3(0, 1, 0);
			if (glm::abs(glm::dot(glm::normalize(target - _localTransform.position), up)) > 0.999f) { up = glm::vec3(0, 0, 1); }
			_localTransform.LookAt(target, up);
			GlobalRotationFromLocal();
		}

		virtual void ScaleLocal(float x, float y, float z) { _localTransform.Scale(x, y, z); GlobalScaleFromLocal(); }
		virtual void ScaleLocal(glm::vec3 scale) { _localTransform.Scale(scale); GlobalScaleFromLocal(); }
		virtual void SetLocalScale(float x, float y, float z) { _localTransform.scale = glm::vec3(x, y, z); GlobalScaleFromLocal(); }
		virtual void SetLocalScale(glm::vec3 scale) { _localTransform.scale = scale; GlobalScaleFromLocal(); }
		virtual void SetLocalUniformScale(float s) { _localTransform.scale = glm::vec3(s, s, s); GlobalScaleFromLocal(); }
		glm::vec3 GetLocalScale() { return _localTransform.scale; }

		glm::vec3 LocalForward() { return _localTransform.forward; }
		glm::vec3 LocalUp() { return _localTransform.up; }
		glm::vec3 LocalRight() { return _localTransform.right; }

		#pragma endregion

		#pragma region Global

		virtual void TranslateGlobal(float x, float y, float z) { _globalTransform.Translate(x, y, z); LocalPositionFromGlobal(); }
		virtual void TranslateGlobal(glm::vec3 vec) { _globalTransform.Translate(vec); LocalPositionFromGlobal(); }
		virtual void SetGlobalPosition(float x, float y, float z) { _globalTransform.position = glm::vec3(x, y, z); LocalPositionFromGlobal(); }
		virtual void SetGlobalPosition(glm::vec3 pos) { _globalTransform.position = pos; LocalPositionFromGlobal(); }
		virtual glm::vec3 GetGlobalPosition() { return _globalTransform.position; }

		virtual void RotateGlobal(float x, float y, float z) { _globalTransform.Rotate(x, y, z); LocalRotationFromGlobal(); }
		virtual void RotateGlobal(glm::vec3 axis, float angle) { _globalTransform.Rotate(axis, angle); LocalRotationFromGlobal(); }
		virtual void RotateAroundGlobal(glm::vec3 axis, glm::vec3 point, float angle) {
			_globalTransform.RotateAround(axis, point, angle);
			LocalRotationFromGlobal();
			LocalPositionFromGlobal();
		}
		virtual void SetGlobalRotation(float x, float y, float z) { _globalTransform.ResetRotation();  _globalTransform.Rotate(x, y, z); LocalRotationFromGlobal(); }
		virtual void ResetGlobalRotation() { _globalTransform.ResetRotation(); LocalRotationFromGlobal(); }

		virtual void LookAtGlobal(glm::vec3 target, glm::vec3 up) { _globalTransform.LookAt(target, up); LocalRotationFromGlobal(); }
		virtual void LookAtGlobal(glm::vec3 target) {
			glm::vec3 up = glm::vec3(0, 1, 0);
			if (glm::abs(glm::dot(glm::normalize(target - _globalTransform.position), up)) > 0.999f) { up = glm::vec3(0, 0, 1); }
			_globalTransform.LookAt(target, up);
			LocalRotationFromGlobal();
		}

		virtual void ScaleGlobal(float x, float y, float z) { _globalTransform.Scale(x, y, z); LocalScaleFromGlobal(); }
		virtual void ScaleGlobal(glm::vec3 scale) { _globalTransform.Scale(scale); LocalScaleFromGlobal(); }
		virtual void SetGlobalScale(float x, float y, float z) { _globalTransform.scale = glm::vec3(x, y, z); LocalScaleFromGlobal(); }
		virtual void SetGlobalScale(glm::vec3 scale) { _globalTransform.scale = scale; LocalScaleFromGlobal(); }
		virtual void SetGlobalUniformScale(float s) { _globalTransform.scale = glm::vec3(s, s, s); LocalScaleFromGlobal(); }
		glm::vec3 GetGlobalScale() { return _globalTransform.scale; }

		glm::vec3 GlobalForward() { return _globalTransform.forward; }
		glm::vec3 GlobalUp() { return _globalTransform.up; }
		glm::vec3 GlobalRight() { return _globalTransform.right; }

		#pragma endregion

		void AddChild(Entity3D* child, bool keepLocal) {
			if (std::find(_children.begin(), _children.end(), child) == _children.end()) {
				_children.push_back(child);
				child->_parent = this;
				if (keepLocal) {
					child->GlobalTransformFromLocal();
				} else {
					child->LocalTransformFromGlobal();
				}
			}
		}

		void RemoveChild(Entity3D* child, bool keepLocal) {
			if (std::find(_children.begin(), _children.end(), child) != _children.end()) {
				_children.remove(child);
				child->_parent = NULL;
				if (keepLocal) {
					child->GlobalTransformFromLocal();
				} else {
					child->LocalTransformFromGlobal();
				}
			}
		}

		virtual void Start() {}
		virtual void Update(double dt) {}
	};
	unsigned int Entity3D::nextId;
}