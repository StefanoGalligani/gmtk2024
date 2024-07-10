#pragma once

#include <unordered_map>
#include <string>
#include <sgStructures.h>

namespace sg {

	class Model {
	private:
		unsigned int _nVertices;
		unsigned int _nMaterials;
		unsigned int _nMeshes;
		Vertex* _vertices;
		Material* _materials;
		Mesh* _meshes;
		glm::vec3 _lowerBound;
		glm::vec3 _upperBound;
		GLuint _vbo;

	public:
		Model() { _nVertices = 0; _nMeshes = 0; _nMaterials = 0; _vertices = NULL;  _meshes = NULL;  _materials = NULL; _vbo = -1; }
		unsigned int GetNVertices() { return _nVertices; }
		unsigned int GetNMaterials() { return _nMaterials; }
		unsigned int GetNMeshes() { return _nMeshes; }
		Vertex* GetVertices() { return _vertices; }
		Vertex GetVertexAt(unsigned int index) { return _vertices[index]; }
		Mesh GetMeshAt(unsigned int index) { return _meshes[index]; }
		Material GetMaterialAt(unsigned int index) { return _materials[index]; }

		glm::vec3 GetBoundingBoxLower() { return _lowerBound; }
		glm::vec3 GetBoundingBoxUpper() { return _upperBound; }
		glm::vec3 GetBoundingBoxCenter() { return (_upperBound+_lowerBound) * 0.5f; }
		void InitFromVerticesAndTriangles(sg::Vertex vertices[], int nVertices, sg::Triangle triangles[], int nTriangles) {
			_vertices = vertices;
			_nVertices = nVertices;

			_materials = new Material[1];
			_materials[0].name = new char[8] {'d', 'e', 'f', 'a', 'u', 'l', 't', '\0'};
			_nMaterials = 1;

			_meshes = new Mesh[1];
			_meshes[0].triangles = triangles;
			_meshes[0].nTriangles = nTriangles;
			_meshes[0].materialName = _materials[0].name;
			_meshes[0].hasMaterial = true;
			_nMeshes = 1;
		}
		void InitFromVerticesMaterialsAndMeshes(sg::Vertex vertices[], int nVertices, sg::Material materials[], int nMaterials, sg::Mesh meshes[], int nMeshes) {
			_vertices = vertices;
			_nVertices = nVertices;

			_materials = new Material[nMaterials];
			for (int i=0; i<nMaterials; i++) _materials[i] = materials[i];
			_nMaterials = nMaterials;

			_meshes = new Mesh[nMeshes];
			for (int i = 0; i < nMaterials; i++) _meshes[i] = meshes[i];
			_nMeshes = nMeshes;
		}
		bool LoadFromObj(char const* filename, bool invertYZ = false);
		void SetVBO(GLuint vao) {
			if (_vbo == -1) {
				glBindVertexArray(vao);
				glGenBuffers(1, &_vbo);
				glBindBuffer(GL_ARRAY_BUFFER, _vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(sg::Vertex) * _nVertices, _vertices, GL_STATIC_DRAW);
			}
		}
		GLuint GetVBO() {
			return _vbo;
		}
		void Destroy() {
			delete(_vertices);
			delete(_meshes);
			delete(_materials);
		}

	private:
		void ClearData() { delete(_vertices); delete(_meshes); delete(_materials); _nVertices = 0; ; _nMaterials = 0; _nMeshes = 0; _lowerBound = glm::vec3(); _upperBound = glm::vec3(); }
		bool ReadMaterial(char const* folder, char const* filename);
		void SeparateFolderFromFilename(char** folder, char const** filename) {
			int lastDiv = -1;
			int i = 0;
			char c;
			while ((c = (*filename)[i]) != '\0') {
				if (c == '/') {
					lastDiv = i;
				}
				i++;
			}
			(*folder) = new char[lastDiv+2];
			for (i = 0; i < lastDiv+1; i++) {
				(*folder)[i] = (*filename)[i];
			}
			(*folder)[lastDiv+1] = '\0';
			(*filename) = (*filename) + lastDiv + 1;
		}
		void UpdateBoundingBox(glm::vec3 coord) {
			if (coord.x < _lowerBound.x) _lowerBound.x = coord.x;
			if (coord.y < _lowerBound.y) _lowerBound.y = coord.y;
			if (coord.z < _lowerBound.z) _lowerBound.z = coord.z;
			if (coord.x > _upperBound.x) _upperBound.x = coord.x;
			if (coord.y > _upperBound.y) _upperBound.y = coord.y;
			if (coord.z > _upperBound.z) _upperBound.z = coord.z;
		}
		FILE* ReadFile(char const* folder, char const* filename) {
			FILE* fp;
			std::string folderStr = folder;
			std::string fileStr = filename;
			fopen_s(&fp, (folderStr + fileStr).c_str(), "r");
			return fp;
		}
		void Concat(char** dest, const char* first, const char* second, bool addSlash = true) {
			char full[50];
			int i = 0;
			char c;
			while ((c = first[i]) != '\0') {
				full[i] = c;
				i++;
			}
			if (addSlash) {
				full[i] = '/';
				i++;
			}
			int j = 0;
			while ((c = *(second + j)) != '\0') {
				full[i + j] = c;
				j++;
			}
			full[i + j] = '\0';
			*dest = new char[i + j + 1];
			strcpy_s(*dest, i+j+1, full);
		}
	};

	class Buffer {
		char data[1024];
		int readLine;
	public:
		int ReadLine(FILE* fp) {
			char c = fgetc(fp);
			while (!feof(fp)) {
				while (isspace(c) && (!feof(fp) || c != '\0')) c = fgetc(fp);	// skip empty space
				if (c == '#') while (!feof(fp) && c != '\n' && c != '\r' && c != '\0') c = fgetc(fp);	// skip comment line
				else break;
			}
			int i = 0;
			bool inspace = false;
			while (i < 1024 - 1) {
				if (feof(fp) || c == '\n' || c == '\r' || c == '\0') break;
				if (isspace(c)) {	// only use a single space as the space character
					inspace = true;
				}
				else {
					if (inspace) data[i++] = ' ';
					inspace = false;
					data[i++] = c;
				}
				c = fgetc(fp);
			}
			data[i] = '\0';
			readLine = i;
			return i;
		}
		char& operator[](int i) { return data[i]; }
		void ReadFloat3(float f[3]) const { f[2] = f[1] = f[0] = 0; int n = sscanf_s(data + 2, "%f %f %f", &f[0], &f[1], &f[2]); if (n == 1) f[2] = f[1] = f[0]; }
		void ReadFloat(float* f) const { sscanf_s(data + 2, "%f", f); }
		void ReadInt(int* i, int start) const { sscanf_s(data + start, "%d", i); }
		bool IsCommand(char const* cmd) const {
			int i = 0;
			while (cmd[i] != '\0') {
				if (cmd[i] != data[i]) return false;
				i++;
			}
			return (data[i] == '\0' || data[i] == ' ');
		}
		char const* Data(int start = 0) { return data + start; }
		void Copy(const char* str, int start = 0) {
			while (data[start] != '\0' && data[start] <= ' ') start++;
			str = Data(start);
		}
		void DeepCopy(char** str, int start = 0) {
			while (data[start] != '\0' && data[start] <= ' ') start++;
			int i = 0;
			while (data[start+i] != '\0') i++;
			*str = new char[i + 1];
			for (int j = 0; j < i+1; j++) {
				(*str)[j] = data[start+j];
			}
		}
	};

	inline bool Model::LoadFromObj(char const* filename, bool invertYZ) {
		printf("Initializing parsing\n");
		std::unordered_map<std::string, unsigned int> map = std::unordered_map<std::string, unsigned int>();
		std::list<Vertex> vertexList = std::list<Vertex>();
		unsigned int indexCounter = 0;
		char* folder;

		SeparateFolderFromFilename(&folder, &filename);

		FILE* fp = ReadFile(folder, filename);
		if (!fp) {
			printf("ERROR: Cannot open file %s\n", filename);
			return false;
		}

		printf("File opened: %s\n", filename);

		ClearData();
		Buffer buffer;
		std::vector<glm::vec3> vCoords;
		std::vector<glm::vec2> vTextures;
		std::vector<glm::vec3> vNormals;

		std::list<Triangle> currentTriangles = std::list<Triangle>();
		std::list<Mesh> meshList;
		int meshCount = 0;
		Mesh* currentMesh = NULL;

		bool needToCreateMesh = true;
		while (int rb = buffer.ReadLine(fp)) {
			if (buffer.IsCommand("mtllib")) {
				const char* mtlFilename = buffer.Data(7);
				ReadMaterial(folder, mtlFilename);
				needToCreateMesh = true;
			} else if (buffer.IsCommand("g") || buffer.IsCommand("o") || (buffer.IsCommand("usemtl") && needToCreateMesh)) {
				printf("Reading new object: ");
				if (currentMesh != NULL) {
					currentMesh -> triangles = new Triangle[currentTriangles.size()];
					currentMesh -> nTriangles = currentTriangles.size();
					int i = 0;
					for (Triangle t : currentTriangles) {
						currentMesh->triangles[i] = t;
						i++;
					}
					currentTriangles.clear();
					meshList.push_back(*currentMesh);
				}
				meshCount++;
				currentMesh = new Mesh();
				buffer.DeepCopy(&(currentMesh->name), 2);
				printf("%s\n", currentMesh->name);
				needToCreateMesh = false;
				if (buffer.IsCommand("usemtl")) {
					currentMesh->hasMaterial = true;
					buffer.DeepCopy(&(currentMesh->materialName), 7);
					needToCreateMesh = true;
				}
			} else if (buffer.IsCommand("usemtl")) {
				currentMesh->hasMaterial = true;
				buffer.DeepCopy(&(currentMesh->materialName), 7);
				needToCreateMesh = true;
			} else if (buffer.IsCommand("v")) {
				float coord[3];
				buffer.ReadFloat3(coord);
				glm::vec3 coordV = glm::vec3(coord[0], coord[1], coord[2]);
				vCoords.push_back(coordV);
			} else if (buffer.IsCommand("vt")) {
				float texture[3];
				buffer.ReadFloat3(texture);
				glm::vec2 textureV = glm::vec2(texture[0], texture[1]);
				vTextures.push_back(textureV);
			} else if (buffer.IsCommand("vn")) {
				float normal[3];
				buffer.ReadFloat3(normal);
				glm::vec3 normalV = glm::vec3(normal[0], normal[1], normal[2]);
				vNormals.push_back(normalV);
			} else if (buffer.IsCommand("f")) {
				const char *line = buffer.Data(2);
				char* triples[4];
				int tripleStart = 0;
				int i = 0;
				int vIndex = 0;
				while (true) {
					if (line[i] == ' ' || line[i] == '\0') {
						triples[vIndex] = new char[i - tripleStart + 1];
						for (int j = tripleStart; j < i; j++) {
							triples[vIndex][j - tripleStart] = line[j];
						}
						triples[vIndex][i - tripleStart] = '\0';
						tripleStart = i;
						vIndex++;
						if (line[i] == '\0') break;
					}
					i++;
				}
				unsigned int faceIndices[4];
				for (i = 0; i < glm::min(vIndex, 4); i++) {
					std::string str = triples[i];
					if (map[str]) {
						faceIndices[i] = map[str];
					} else {
						faceIndices[i] = indexCounter;
						map[str] = indexCounter;
						indexCounter++;

						int infoIndices[3];
						sscanf_s(triples[i], "%d/%d/%d", &infoIndices[0], &infoIndices[1], &infoIndices[2]);
						infoIndices[0]--; infoIndices[1]--; infoIndices[2]--;

						Vertex v = Vertex();
						if (invertYZ) {
							v.coord.x = { vCoords[infoIndices[0]].x };
							v.coord.y = { vCoords[infoIndices[0]].z };
							v.coord.z = { vCoords[infoIndices[0]].y };
							v.texture.x = { vTextures[infoIndices[1]].x };
							v.texture.y = { vTextures[infoIndices[1]].y };
							v.normal.x = { vNormals[infoIndices[2]].x };
							v.normal.y = { vNormals[infoIndices[2]].z };
							v.normal.z = { vNormals[infoIndices[2]].y };
						} else {
							v.coord.x = { vCoords[infoIndices[0]].x };
							v.coord.y = { vCoords[infoIndices[0]].y };
							v.coord.z = { vCoords[infoIndices[0]].z };
							v.texture.x = { vTextures[infoIndices[1]].x };
							v.texture.y = { vTextures[infoIndices[1]].y };
							v.normal.x = { vNormals[infoIndices[2]].x };
							v.normal.y = { vNormals[infoIndices[2]].y };
							v.normal.z = { vNormals[infoIndices[2]].z };
						}
						UpdateBoundingBox(v.coord);
						vertexList.push_back(v);
					}
				}

				if (vIndex == 3) {
					Triangle t = Triangle();
					t.index[0] = faceIndices[0]; t.index[1] = faceIndices[1]; t.index[2] = faceIndices[2];
					currentTriangles.push_back(t);
				} else if (vIndex == 4) {
					Triangle t1 = Triangle();
					Triangle t2 = Triangle();
					t1.index[0] = faceIndices[0]; t1.index[1] = faceIndices[1]; t1.index[2] = faceIndices[2];
					t2.index[0] = faceIndices[0]; t2.index[1] = faceIndices[2]; t2.index[2] = faceIndices[3];
					currentTriangles.push_back(t1);
					currentTriangles.push_back(t2);
				}
			}
		}
		if (currentMesh != NULL) {
			currentMesh->triangles = new Triangle[currentTriangles.size()];
			currentMesh->nTriangles = currentTriangles.size();
			int i = 0;
			for (Triangle t : currentTriangles) {
				currentMesh->triangles[i] = t;
				i++;
			}
			meshList.push_back(*currentMesh);
		}
		printf("File read\n");
		_vertices = new Vertex[vertexList.size()];
		_nVertices = vertexList.size();
		int i = 0;
		for (Vertex v : vertexList) {
			_vertices[i] = v;
			i++;
		}
		_meshes = new Mesh[meshList.size()];
		_nMeshes = meshList.size();
		i = 0;
		for (Mesh m : meshList) {
			_meshes[i] = m;
			i++;
		}
		printf("Parsing completed: %d vertices\n", _nVertices);
		return true;
	}

	inline bool Model::ReadMaterial(char const* folder, char const* filename) {
		printf("Reading materials\n");
		FILE* fp = ReadFile(folder, filename);
		if (!fp) {
			printf("ERROR: Cannot open file %s\n", filename);
			return false;
		}

		Buffer buffer;
		std::list<Material> matList;
		int matCount = 0;
		Material* currentMat = NULL;

		std::string folderStr = folder;
		while (int rb = buffer.ReadLine(fp)) {
			if (buffer.IsCommand("newmtl")) {
				if (currentMat != NULL) matList.push_back(*currentMat);
				matCount++;
				currentMat = new Material();
				buffer.DeepCopy(&(currentMat->name), 7);
			}
			else if (buffer.IsCommand("Ka")) {
				buffer.ReadFloat3(currentMat->Ka);
			}
			else if (buffer.IsCommand("Kd")) {
				buffer.ReadFloat3(currentMat->Kd);
			}
			else if (buffer.IsCommand("Ks")) {
				buffer.ReadFloat3(currentMat->Ks);
			}
			else if (buffer.IsCommand("Ke")) {
				buffer.ReadFloat3(currentMat->Ke);
			}
			else if (buffer.IsCommand("Tf")) {
				buffer.ReadFloat3(currentMat->Tf);
			}
			else if (buffer.IsCommand("Ns")) {
				buffer.ReadFloat(&(currentMat->Ns));
			}
			else if (buffer.IsCommand("Ni")) {
				buffer.ReadFloat(&(currentMat->Ni));
			}
			else if (buffer.IsCommand("illum")) {
				buffer.ReadInt(&(currentMat->illum), 5);
			}
			else if (buffer.IsCommand("d")) {
				buffer.ReadFloat(&(currentMat->d));
			}
			else if (buffer.IsCommand("Tr")) {
				buffer.ReadFloat(&(currentMat->Tr));
			}
			else if (buffer.IsCommand("map_Ka")) {
				Texture t_ka = Texture();
				Concat(&(t_ka.map), folder, buffer.Data(7));
				t_ka.isPresent = true;
				currentMat->texture_Ka = t_ka;
			}
			else if (buffer.IsCommand("map_Kd")) {
				Texture t_kd = Texture();
				Concat(&(t_kd.map), folder, buffer.Data(7), false);
				t_kd.isPresent = true;
				currentMat->texture_Kd = t_kd;
			}
			else if (buffer.IsCommand("map_Ks")) {
				Texture t_ks = Texture();
				Concat(&(t_ks.map), folder, buffer.Data(7));
				t_ks.isPresent = true;
				currentMat->texture_Ks = t_ks;
			}
		}
		if (currentMat != NULL) matList.push_back(*currentMat);

		_nMaterials = matCount;
		_materials = new Material[_nMaterials];
		int i = 0;
		for (Material m : matList) {
			_materials[i] = m;
			i++;
		}
		printf("Materials read: %d\n", _nMaterials);
		return true;
	}

}