#pragma once
#include <sgModel.h>
#include <sgTextureManager.h>
#include <sgTransform.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <glm/glm/gtx/vector_angle.hpp>

namespace sg {
	class Object3D {
	private:
		static unsigned int nextId;
		int _id;
		glm::mat4 _modelMatrix;
		Transform _transform;
		Model* _model3D;
		int _patches;
		int _childrenCount;
		int _childrenArraySize;
		Object3D *_children;
		GLuint _vbo;

		glm::mat4 BuildRotationMatrix() {
			return glm::inverse(glm::lookAt(glm::vec3(0), _transform.forward, _transform.up));
		}

		void BuildModelMatrix() {
			_modelMatrix = glm::translate(_transform.localPosition)
				* BuildRotationMatrix()
				* glm::scale(_transform.localScale);
		}

		void CreateChildrenArray(int n) {
			_childrenArraySize = n;
			_children = (Object3D*)malloc(sizeof(Object3D) * n);
		}

		void IncreaseChildrenArraySize() {
			Object3D* _newPtr = (Object3D*)malloc(sizeof(Object3D) * 2 * _childrenArraySize);
			for (int i = 0; i < _childrenCount; i++) {
				_newPtr[i] = _children[i];
			}
			free(_children);
			_childrenArraySize *= 2;
			_children = _newPtr;
		}

		void DecreaseChildrenArraySize() {
			Object3D* _newPtr = (Object3D*)malloc(sizeof(Object3D) * _childrenArraySize / 2);
			for (int i = 0; i < _childrenCount; i++) {
				_newPtr[i] = _children[i];
			}
			free(_children);
			_childrenArraySize /= 2;
			_children = _newPtr;
		}

		bool FrustumCheck(glm::mat4 viewProjection) {
			return true;
		}

	public:
		bool CastsShadows;
		bool ReceivesShadows;
		bool Lit;

		Object3D() : _id(nextId++) {
			_modelMatrix = glm::mat4(1);
			_transform = Transform();
			_model3D = NULL;
			_childrenCount = 0;
			_childrenArraySize = 0;
			_children = NULL;
			_vbo = -1;
			_patches = 0;
			CastsShadows = false;
			ReceivesShadows = false;
			Lit = false;
		}

		void SetVbo(GLuint vbo) {
			_vbo = vbo;
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(sg::Vertex) * _model3D->GetNVertices(), _model3D->GetVertices(), GL_STATIC_DRAW);
		}

		bool LoadModelFromObj(const char* path) {
			_model3D = (Model*)malloc(sizeof(Model));
			*_model3D = Model();
			return _model3D->LoadFromObj(path);
		}

		void LoadModelFromData(sg::Vertex vertices[], int nVertices, sg::Triangle triangles[], int nTriangles) {
			_model3D = (Model*)malloc(sizeof(Model));
			*_model3D = Model();
			_model3D->InitFromVerticesAndTriangles(vertices, nVertices, triangles, nTriangles);
		}

		void LoadModelFromData(sg::Vertex vertices[], int nVertices, sg::Material materials[], int nMaterials, sg::Mesh meshes[], int nMeshes) {
			_model3D = (Model*)malloc(sizeof(Model));
			*_model3D = Model();
			_model3D->InitFromVerticesMaterialsAndMeshes(vertices, nVertices, materials, nMaterials, meshes, nMeshes);
		}

		void SetModel(sg::Model* model) {
			_model3D = model;
		}

		void SetPatches(int p) {
			_patches = p;
		}

		Model* GetModel() {
			return _model3D;
		}

		glm::mat4 GetModelMatrix() {
			BuildModelMatrix();
			return _modelMatrix;
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

		void AddChild(Object3D child) {
			if (_childrenArraySize == 0) CreateChildrenArray(2);
			if (_childrenCount == _childrenArraySize) IncreaseChildrenArraySize();
			_children[_childrenCount] = child;
			_childrenCount++;
		}

		bool RemoveChild(Object3D child) {
			for (int i = 0; i < _childrenCount; i++) {
				if (_children[i]._id == child._id) {
					for (int j = i; j < _childrenCount-1; j++) {
						_children[j] = _children[j + 1];
					}
					_childrenCount--;
					if (_childrenCount < _childrenArraySize / 4) DecreaseChildrenArraySize();
					return true;
				}
			}
			return false;
		}

		void Draw(glm::mat4 vp, GLuint program) {
			BuildModelMatrix();
			glm::mat4 mvp = vp * _modelMatrix;
			if (FrustumCheck(vp)) {
				glUseProgram(program);
				glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, false, glm::value_ptr(mvp));

				glBindBuffer(GL_ARRAY_BUFFER, _vbo);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)(sizeof(float) * 3));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)(sizeof(float) * 5));
				for (int i = 0; i < _model3D->GetNMeshes(); i++) {
					sg::Mesh m = _model3D->GetMeshAt(i);
					sg::TextureManager::Instance()->SetMaterialData(program, _model3D->GetMaterialByName(m.materialName));
					if (_patches > 0) {
						glDrawArrays(GL_PATCHES, 0, _patches);
					} else {
						glDrawElements(GL_TRIANGLES, m.nTriangles * 3, GL_UNSIGNED_INT, m.triangles);
					}
				}
			}

			for (int i = 0; i < _childrenCount; i++) {
				_children[i].Draw(mvp, program);
			}
		}
	};
	unsigned int Object3D::nextId;

}
