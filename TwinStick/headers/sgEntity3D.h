#pragma once
#include <sgTransform.h>
#include <list>

namespace sg {
	class Entity3D {
	protected:
		Transform _localTransform;
		Transform _globalTransform;
		Entity3D* _parent;

	private:
		static unsigned int nextId;
		int _id;
		std::list<Entity3D*> _children;

		void LocalTransformFromGlobal() {
			LocalPositionFromGlobal(false);
			LocalRotationFromGlobal(false);
			LocalScaleFromGlobal(false);
			UpdateGlobalTransformInChildren();
		}

		void GlobalTransformFromLocal() {
			GlobalPositionFromLocal(false);
			GlobalRotationFromLocal(false);
			GlobalScaleFromLocal(false);
			UpdateGlobalTransformInChildren();
		}

		void LocalPositionFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.position = _globalTransform.position;
				return;
			}
			//TODO
			//dipende da posizione globale, posizione, rotazione e scala del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void LocalRotationFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.forward = _globalTransform.forward;
				_localTransform.right = _globalTransform.right;
				_localTransform.up = _globalTransform.up;
				return;
			}
			//TODO
			//dipende da rotazione globale e rotazione del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void LocalScaleFromGlobal(bool updateChildren = true) {
			if (_parent == NULL) {
				_localTransform.scale = _globalTransform.scale;
				return;
			}
			//TODO
			//dipende da scala globale e scala del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void GlobalPositionFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.position = _localTransform.position;
				return;
			}
			//TODO
			//dipende da posizione locale e posizione, rotazione e scala del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void GlobalRotationFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.forward = _localTransform.forward;
				_globalTransform.right = _localTransform.right;
				_globalTransform.up = _localTransform.up;
				return;
			}
			//TODO
			//dipende da rotazione locale e rotazione del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void GlobalScaleFromLocal(bool updateChildren = true) {
			if (_parent == NULL) {
				_globalTransform.scale = _localTransform.scale;
				return;
			}
			//TODO
			//dipende da scala locale e scala del parent

			if (updateChildren) UpdateGlobalTransformInChildren();
		}

		void UpdateGlobalTransformInChildren() {
			for (auto const& child : _children) {
				child->GlobalTransformFromLocal();
			}
		}

	public:

		Entity3D() : _id(nextId++) {
			_localTransform = Transform();
			_globalTransform = Transform();
			_children = std::list<Entity3D*>();
			_parent = NULL;
		}

		#pragma region Local

		void TranslateLocal(float x, float y, float z) { _localTransform.Translate(x, y, z); GlobalPositionFromLocal(); }
		void TranslateLocal(glm::vec3 vec) { _localTransform.Translate(vec); GlobalPositionFromLocal(); }
		void SetLocalPosition(float x, float y, float z) { _localTransform.position = glm::vec3(x, y, z); GlobalPositionFromLocal(); }
		void SetLocalPosition(glm::vec3 pos) { _localTransform.position = pos; GlobalPositionFromLocal(); }
		glm::vec3 GetLocalPosition() { return _localTransform.position; }

		void RotateLocal(float x, float y, float z) { _localTransform.Rotate(x, y, z); GlobalRotationFromLocal(); }
		void RotateLocal(glm::vec3 axis, float angle) { _localTransform.Rotate(axis, angle); GlobalRotationFromLocal(); }
		void RotateAroundLocal(glm::vec3 axis, glm::vec3 point, float angle) {
			_localTransform.RotateAround(axis, point, angle);
			GlobalRotationFromLocal();
			GlobalPositionFromLocal();
		}
		void SetLocalRotation(float x, float y, float z) { _localTransform.ResetRotation();  _localTransform.Rotate(x, y, z); GlobalRotationFromLocal(); }
		void ResetLocalRotation() { _localTransform.ResetRotation(); GlobalRotationFromLocal(); }

		void LookAtLocal(glm::vec3 target, glm::vec3 up) { _localTransform.LookAt(target, up); GlobalRotationFromLocal(); }
		void LookAtLocal(glm::vec3 target) { _localTransform.LookAt(target, glm::vec3(0, 1, 0)); GlobalRotationFromLocal(); }

		void ScaleLocal(float x, float y, float z) { _localTransform.Scale(x, y, z); GlobalScaleFromLocal(); }
		void ScaleLocal(glm::vec3 scale) { _localTransform.Scale(scale); GlobalScaleFromLocal(); }
		void SetLocalScale(float x, float y, float z) { _localTransform.scale = glm::vec3(x, y, z); GlobalScaleFromLocal(); }
		void SetLocalScale(glm::vec3 scale) { _localTransform.scale = scale; GlobalScaleFromLocal(); }
		void SetLocalUniformScale(float s) { _localTransform.scale = glm::vec3(s, s, s); GlobalScaleFromLocal(); }

		glm::vec3 LocalForward() { return _localTransform.forward; }
		glm::vec3 LocalUp() { return _localTransform.up; }
		glm::vec3 LocalRight() { return _localTransform.right; }

		#pragma endregion

		#pragma region Global

		void TranslateGlobal(float x, float y, float z) { _globalTransform.Translate(x, y, z); LocalPositionFromGlobal(); }
		void TranslateGlobal(glm::vec3 vec) { _globalTransform.Translate(vec); LocalPositionFromGlobal(); }
		void SetGlobalPosition(float x, float y, float z) { _globalTransform.position = glm::vec3(x, y, z); LocalPositionFromGlobal(); }
		void SetGlobalPosition(glm::vec3 pos) { _globalTransform.position = pos; LocalPositionFromGlobal(); }
		glm::vec3 GetGlobalPosition() { return _globalTransform.position; }

		void RotateGlobal(float x, float y, float z) { _globalTransform.Rotate(x, y, z); LocalRotationFromGlobal(); }
		void RotateGlobal(glm::vec3 axis, float angle) { _globalTransform.Rotate(axis, angle); LocalRotationFromGlobal(); }
		void RotateAroundGlobal(glm::vec3 axis, glm::vec3 point, float angle) {
			_globalTransform.RotateAround(axis, point, angle);
			LocalRotationFromGlobal();
			LocalPositionFromGlobal();
		}
		void SetGlobalRotation(float x, float y, float z) { _globalTransform.ResetRotation();  _globalTransform.Rotate(x, y, z); LocalRotationFromGlobal(); }
		void ResetGlobalRotation() { _globalTransform.ResetRotation(); LocalRotationFromGlobal(); }

		void LookAtGlobal(glm::vec3 target, glm::vec3 up) { _globalTransform.LookAt(target, up); LocalRotationFromGlobal(); }
		void LookAtGlobal(glm::vec3 target) { _globalTransform.LookAt(target, glm::vec3(0, 1, 0)); LocalRotationFromGlobal(); }

		void ScaleGlobal(float x, float y, float z) { _globalTransform.Scale(x, y, z); LocalScaleFromGlobal(); }
		void ScaleGlobal(glm::vec3 scale) { _globalTransform.Scale(scale); LocalScaleFromGlobal(); }
		void SetGlobalScale(float x, float y, float z) { _globalTransform.scale = glm::vec3(x, y, z); LocalScaleFromGlobal(); }
		void SetGlobalScale(glm::vec3 scale) { _globalTransform.scale = scale; LocalScaleFromGlobal(); }
		void SetGlobalUniformScale(float s) { _globalTransform.scale = glm::vec3(s, s, s); LocalScaleFromGlobal(); }

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

		void Start() {}
		void Update(double dt) {}
	};
	unsigned int Entity3D::nextId;
}