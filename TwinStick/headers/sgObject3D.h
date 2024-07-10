#pragma once
#include <sgEntity3D.h>
#include <sgModel.h>
#include <sgTextureManager.h>

namespace sg {
	class Object3D : public Entity3D {
	private:
		Model* _model3D = NULL;
		Material* _materials = NULL;
		unsigned int _nMaterials;
		int _patches;
		glm::mat4 _modelMatrix;
		bool _copiedModel;

		bool FrustumCheck(glm::mat4 mvp) {
			glm::vec3 points[8];
			glm::vec3 offset = glm::vec3(0.05, 0.05, 0.05);
			points[0] = _model3D->GetBoundingBoxLower() - offset;
			points[7] = _model3D->GetBoundingBoxUpper() + offset;
			glm::vec3 lower = glm::vec3(2, 2, 2);
			glm::vec3 upper = glm::vec3(-2, -2, -2);
			for (int i = 1; i < 7; i++) {
				points[i] = glm::vec3(points[7 * ((i / 4) % 2)].x, points[7 * ((i / 2) % 2)].y, points[7 * (i % 2)].z);
			}
			for (int i = 0; i < 8; i++) {
				glm::vec4 v = mvp * glm::vec4(points[i], 1);
				v.x = v.x / v.w; v.y = v.y / v.w; v.z = v.z / v.w;
				if (v.x < lower.x) lower.x = v.x;
				if (v.y < lower.y) lower.y = v.y;
				if (v.z < lower.z) lower.z = v.z;
				if (v.x > upper.x) upper.x = v.x;
				if (v.y > upper.y) upper.y = v.y;
				if (v.z > upper.z) upper.z = v.z;
			}
			if (lower.x > 1 || lower.y > 1 || lower.z > 1 || upper.x < -1 || upper.y < -1 || upper.z < -1) {
				//printf("Model not rendered: %s\n", _model3D->GetMeshAt(0).name);
				return false;
			}
			return true;
		}

		void CopyMaterialsFromModel() {
			_nMaterials = _model3D->GetNMaterials();
			_materials = (Material*)malloc(sizeof(Material) * _nMaterials);
			for (int i = 0; i < _nMaterials; i++) {
				_materials[i] = _model3D->GetMaterialAt(i);
			}
		}

		glm::mat4 BuildRotationMatrix() {
			return glm::inverse(glm::lookAt(glm::vec3(0), GlobalForward(), GlobalUp()));
		}

		void BuildModelMatrix() {
			_modelMatrix = glm::translate(GetGlobalPosition())
				* BuildRotationMatrix()
				* glm::scale(_globalTransform.scale);
		}

	public:
		bool CastsShadows;
		bool ReceivesShadows;
		bool Lit;
		bool PerformFrustumCheck;

		Object3D() : Entity3D() {
			_modelMatrix = glm::mat4(1);
			_model3D = NULL;
			_patches = 0;
			_copiedModel = false;
			CastsShadows = false;
			ReceivesShadows = false;
			Lit = false;
			PerformFrustumCheck = true;
		}

		glm::mat4 GetModelMatrix() {
			BuildModelMatrix();
			return _modelMatrix;
		}

		bool LoadModelFromObj(const char* path) {
			_model3D = new Model();
			if (_model3D->LoadFromObj(path)) {
				CopyMaterialsFromModel();
				return true;
			}
			return false;
		}

		void LoadModelFromData(sg::Vertex vertices[], int nVertices, sg::Triangle triangles[], int nTriangles) {
			_model3D = new Model();
			_model3D->InitFromVerticesAndTriangles(vertices, nVertices, triangles, nTriangles);
			CopyMaterialsFromModel();
		}

		void LoadModelFromData(sg::Vertex vertices[], int nVertices, sg::Material materials[], int nMaterials, sg::Mesh meshes[], int nMeshes) {
			_model3D = new Model();
			_model3D->InitFromVerticesMaterialsAndMeshes(vertices, nVertices, materials, nMaterials, meshes, nMeshes);
			CopyMaterialsFromModel();
		}

		void SetModel(sg::Model* model) {
			_model3D = model;
			CopyMaterialsFromModel();
			_copiedModel = true;
		}

		Material GetMaterialAt(unsigned int index) { return _materials[index]; }

		Material* GetMaterialReferenceAt(unsigned int index) { return &_materials[index]; }

		Material* GetMaterialByName(const char* name) {
			for (int i = 0; i < _nMaterials; i++) {
				if (strcmp(name, _materials[i].name) == 0) {
					return &_materials[i];
				}
			}
			return &_materials[0];
		}

		void ChangeMaterial(const char* name, Material newMat) {
			Material* old = GetMaterialByName(name);
			old->Ka[0] = newMat.Ka[0]; old->Ka[1] = newMat.Ka[1];  old->Ka[2] = newMat.Ka[2];
			old->Kd[0] = newMat.Kd[0]; old->Kd[1] = newMat.Kd[1];  old->Kd[2] = newMat.Kd[2];
			old->Ks[0] = newMat.Ks[0]; old->Ks[1] = newMat.Ks[1];  old->Ks[2] = newMat.Ks[2];
			old->Ke[0] = newMat.Ke[0]; old->Ke[1] = newMat.Ke[1];  old->Ke[2] = newMat.Ke[2];
			old->Tf[0] = newMat.Tf[0]; old->Tf[1] = newMat.Tf[1];  old->Tf[2] = newMat.Tf[2];
			old->Ns = newMat.Ns;
			old->Ni = newMat.Ni;
			old->illum = newMat.illum;
			old->d = newMat.d;
			old->Tr = newMat.Tr;
			old->texture_Ka = newMat.texture_Ka;
			old->texture_Kd = newMat.texture_Kd;
			old->texture_Ks = newMat.texture_Ks;
			old->texture_Ns = newMat.texture_Ns;
			old->texture_d = newMat.texture_d;
			old->texture_bump = newMat.texture_bump;
			old->texture_disp = newMat.texture_disp;
		}

		void SetPatches(int p) {
			_patches = p;
		}

		Model* GetModel() {
			return _model3D;
		}

		void Draw(glm::mat4 vp, GLuint program) {
			BuildModelMatrix();
			glm::mat4 mvp = vp * _modelMatrix;
			if (!PerformFrustumCheck || FrustumCheck(mvp)) {
				glUseProgram(program);
				glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, false, glm::value_ptr(mvp));

				glBindBuffer(GL_ARRAY_BUFFER, _model3D->GetVBO());
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)(sizeof(float) * 3));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)(sizeof(float) * 5));
				for (int i = 0; i < _model3D->GetNMeshes(); i++) {
					sg::Mesh m = _model3D->GetMeshAt(i);
					sg::TextureManager::Instance()->SetMaterialData(program, GetMaterialByName(m.materialName));
					if (_patches > 0) {
						glDrawArrays(GL_PATCHES, 0, _patches);
					} else {
						glDrawElements(GL_TRIANGLES, m.nTriangles * 3, GL_UNSIGNED_INT, m.triangles);
					}
				}
			}
		}

		~Object3D() {
			if (!_copiedModel) {
				if (_model3D) _model3D->Destroy();
				delete(_model3D);
			}
			free(_materials);
		}
	};
}
