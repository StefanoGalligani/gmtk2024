#pragma once

#include <GL/glew.h>
#include <glm/glm/glm.hpp>

namespace sg {

	struct FrameBuffer {
		GLuint bufferIndex = 0;
		bool hasTexture = false;
		GLuint renderTexture = 0;
		bool hasDepthMap = false;
		GLuint depthMap = 0;
		bool hasDepth = false;
		GLuint depthBuffer = 0;
		bool isRectangle = false;
		bool isValid = false;

		sg::FrameBuffer(float width, float height, bool createTexture = true, bool createDepthMap = false, bool createDepthBuffer = true, bool rectangleTexture = false) {
			isRectangle = rectangleTexture;
			glGenFramebuffers(1, &bufferIndex);
			glBindFramebuffer(GL_FRAMEBUFFER, bufferIndex);

			if (createTexture) {
				hasTexture = true;
				glGenTextures(1, &renderTexture);
				glActiveTexture(GL_TEXTURE0);
				GLuint textureType = rectangleTexture ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
				glBindTexture(textureType, renderTexture);
				glTexImage2D(textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY, 5);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
			}
			if (createDepthMap) {
				hasDepthMap = true;
				glGenTextures(1, &depthMap);
				glActiveTexture(GL_TEXTURE0);
				GLuint textureType = rectangleTexture ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
				glBindTexture(textureType, depthMap);
				glTexImage2D(textureType, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(textureType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTexParameteri(textureType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColor);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
			}
			if (createDepthBuffer) {
				hasDepth = true;
				glGenRenderbuffers(1, &depthBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			}

			if (hasTexture) {
				GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, drawBuffers);
			}
			else {
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}

			this->isValid = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}
	};

	struct Vertex {
		glm::vec3 coord;
		glm::vec2 texture;
		glm::vec3 normal;
	};

	struct Triangle {
		unsigned int index[3];
	};

	struct Texture {
		char* map;
		bool isPresent;
		bool isLoaded;
		GLuint index;

		sg::Texture() {
			map = NULL;
			isPresent = false;
			isLoaded = false;
			index = -1;
		}

		sg::Texture(char* name) {
			map = name;
			isPresent = false;
			isLoaded = false;
			index = -1;
		}

		sg::Texture(GLuint ind) {
			map = NULL;
			isPresent = true;
			isLoaded = false;
			index = ind;
		}
	};

	struct Material {
		char* name;
		float Kd[3];
		float Ks[3];
		float Ke[3];
		float Tf[3];
		float Ns;
		float Ni;
		int illum;
		float d;
		float Tr;
		Texture texture_Kd;
		Texture texture_Ks;
		Texture texture_Ns;
		Texture texture_d;
		Texture texture_bump;
		Texture texture_disp;

		sg::Material() {
			name = NULL;
			Kd[0] = Kd[1] = Kd[2] = 1;
			Ks[0] = Ks[1] = Ks[2] = 0;
			Ke[0] = Ke[1] = Ke[2] = 0;
			Tf[0] = Tf[1] = Tf[2] = 0;
			Ns = 0;
			Ni = 1;
			illum = 2;
			d = 1;
			Tr = 0;
			texture_Kd = Texture();
			texture_Ks = Texture();
			texture_Ns = Texture();
			texture_d = Texture();
			texture_bump = Texture();
			texture_disp = Texture();
		}

		sg::Material(char* n, float diff[3], float spec[3], float sharpness, float transp) {
			name = n;
			Kd[0] = diff[0]; Kd[1] = diff[1]; Kd[2] = diff[2];
			Ks[0] = spec[0]; Ks[1] = spec[1]; Ks[2] = spec[2];
			Ke[0] = Ke[1] = Ke[2] = 0;
			Tf[0] = Tf[1] = Tf[2] = 0;
			Ns = sharpness;
			Ni = 1;
			illum = 2;
			d = transp;
			Tr = 0;
			texture_Kd = Texture();
			texture_Ks = Texture();
			texture_Ns = Texture();
			texture_d = Texture();
			texture_bump = Texture();
			texture_disp = Texture();
		}
	};

	struct Mesh {
		char* name;
		bool hasMaterial;
		char* materialName;
		sg::Triangle *triangles;
		int nTriangles;

		sg::Mesh() {
			name = NULL;
			hasMaterial = false;
			materialName = NULL;
			triangles = NULL;
			nTriangles = 0;
		}

		sg::Mesh(char* n, char* matName, sg::Triangle* tris, int nTris) {
			name = n;
			hasMaterial = true;
			materialName = matName;
			triangles = tris;
			nTriangles = nTris;
		}
	};

	struct Polar {
		double anglex;
		double angley;
		float distance;

		Polar(double anglex, double angley, double distance) {
			this->anglex = anglex;
			this->angley = angley;
			this->distance = distance;
		}

		glm::vec3 GetCoordinates() {
			return distance * glm::vec3(cos(angley) * sin(anglex), sin(angley), cos(angley) * cos(anglex));
		}

		void Rotate(double movex, double movey) {
			anglex += movex;
			angley += movey;
			if (angley > 3.141 / 2) angley = 3.141 / 2;
			if (angley < -3.141 / 2) angley = -3.141 / 2;
		}
	};

}