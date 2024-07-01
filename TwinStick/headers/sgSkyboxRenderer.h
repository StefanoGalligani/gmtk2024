#pragma once
#include <sgCamera3D.h>

namespace sg {
    class SkyboxRenderer {
    private:
        GLuint _backgroundProgram;
        GLuint _skyboxTexture = -1;
        Vertex _backgroundVertices[3];
        Triangle _backgroundTriangles[1];
        GLuint _backgroundVBO = -1;
        bool _isPresent = false;

    public:
        void InitSkybox(const char* textureFaces[6], GLuint vao) {
            _backgroundProgram = sg::CreateProgram("shaders/vertexShader_background.glsl", "shaders/fragmentShader_background.glsl");

            _skyboxTexture = TextureManager::Instance()->SetCubemap(textureFaces);

            _backgroundVertices[0] = sg::Vertex{ glm::vec3(-1, -1, 1 - 1e-5), glm::vec2(0, 0), glm::vec3(0,0,1) };
            _backgroundVertices[1] = sg::Vertex{ glm::vec3(-1, 3, 1 - 1e-5), glm::vec2(0, 1), glm::vec3(0,0,1) };
            _backgroundVertices[2] = sg::Vertex{ glm::vec3(3, -1, 1 - 1e-5), glm::vec2(1, 0), glm::vec3(0,0,1) };
            _backgroundTriangles[0] = sg::Triangle{ {0,2,1} };

            glBindVertexArray(vao);
            glGenBuffers(1, &_backgroundVBO);
            glBindBuffer(GL_ARRAY_BUFFER, _backgroundVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(sg::Vertex) * 3, _backgroundVertices, GL_STATIC_DRAW);

            _isPresent = true;
        }

        bool IsPresent() {
            return _isPresent;
        }

        void RenderSkybox(Camera3D* camera) {
            glUseProgram(_backgroundProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxTexture);
            glUniform1i(glGetUniformLocation(_backgroundProgram, "skybox"), 0);
            glUniform1i(glGetUniformLocation(_backgroundProgram, "skyboxSet"), 1);

            glm::mat3 matrixPV = glm::inverse(glm::mat3(camera->GetViewProjection()));
            glUniformMatrix3fv(glGetUniformLocation(_backgroundProgram, "toWorld"), 1, false, glm::value_ptr(matrixPV));
            glBindBuffer(GL_ARRAY_BUFFER, _backgroundVBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sg::Vertex), (GLvoid*)0);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, _backgroundTriangles);
        }
    };
}