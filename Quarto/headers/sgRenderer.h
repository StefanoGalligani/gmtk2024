#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sgUtils.h>
#include <sgObject3D.h>
#include <sgSpotLight3D.h>
#include <sgCamera3D.h>
#include <thread>

namespace sg {
	class Renderer {
    private:
        GLuint _shadowedProgram;
        GLuint _depthProgram;
        GLuint _unlitProgram;
        GLuint _triangulationProgram;
        bool _showTriangulation;

        GLFWwindow* _window;
        GLint _origFB;
        int _width;
        int _height;
        GLuint _vao;

        Camera3D* _mainCamera;
        SpotLight3D* _spotLight;
        Object3D* _objects[2];
        int _objectsCount = 0;

        double _timestep = 1000.0 / 40;
        int _tessellationLevel = 1;

        int _textureCount = 0;

    public:
        int InitWindow(const char* title, int width, int height) {
            /* Initialize the library */
            if (!glfwInit())
                return -1;

            /* Create a windowed mode window and its OpenGL context */
            _window = glfwCreateWindow(width, height, title, NULL, NULL);
            if (!_window)
            {
                glfwTerminate();
                return -1;
            }

            /* Make the window's context current */
            glfwMakeContextCurrent(_window);

            if (glewInit() != GLEW_OK) {
                std::cout << "Error with Glew" << std::endl;
                return -1;
            }
            std::cout << glGetString(GL_VERSION) << std::endl;

            glClearColor(0, 0, 0, 0);
            glViewport(0, 0, width, height);

            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &_origFB);
            _width = width;
            _height = height;

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glPatchParameteri(GL_PATCH_VERTICES, 4);
            glEnable(GL_DEPTH_TEST);

            return 0;
        }

        void SetDisplacementUniforms(int textureUnit) {
            glUseProgram(_shadowedProgram);
            glUniform1i(glGetUniformLocation(_shadowedProgram, "displacementTexture"), textureUnit);
            glUniform1i(glGetUniformLocation(_shadowedProgram, "displacementTextureSet"), 1);
            glUniform1f(glGetUniformLocation(_shadowedProgram, "displacementValue"), 8);
            glUseProgram(_triangulationProgram);
            glUniform1i(glGetUniformLocation(_triangulationProgram, "displacementTexture"), textureUnit);
            glUniform1i(glGetUniformLocation(_triangulationProgram, "displacementTextureSet"), 1);
            glUniform1f(glGetUniformLocation(_triangulationProgram, "displacementValue"), 8);
            glUseProgram(_depthProgram);
            glUniform1i(glGetUniformLocation(_depthProgram, "displacementTexture"), textureUnit);
            glUniform1i(glGetUniformLocation(_depthProgram, "displacementTextureSet"), 1);
            glUniform1f(glGetUniformLocation(_depthProgram, "displacementValue"), 8);
        }

        void UpdateTessellationLevel(int newLevel) {
            _tessellationLevel = newLevel;
            glUseProgram(_triangulationProgram);
            glUniform1f(glGetUniformLocation(_triangulationProgram, "level"), _tessellationLevel);
            glUseProgram(_shadowedProgram);
            glUniform1f(glGetUniformLocation(_shadowedProgram, "level"), _tessellationLevel);
            glUseProgram(_depthProgram);
            glUniform1f(glGetUniformLocation(_depthProgram, "level"), _tessellationLevel);
        }

        void InitPrograms() {
            _shadowedProgram = sg::CreateProgram("shaders/vertexShader_normalmapping.glsl", "shaders/fragmentShader_normalmapping.glsl"
                , "shaders/tessellationControlShader_normalMapping.glsl", "shaders/tessellationEvaluationShader_normalMapping.glsl"
            );
            _depthProgram = sg::CreateProgram("shaders/vertexShader_depth.glsl", "shaders/fragmentShader_depth.glsl"
                , "shaders/tessellationControlShader_quad.glsl", "shaders/tessellationEvaluationShader_quad.glsl"
            );
            _unlitProgram = sg::CreateProgram("shaders/vertexShader_unlit.glsl", "shaders/fragmentShader_unlit.glsl");
            _triangulationProgram = sg::CreateProgram("shaders/vertexShader_triangulation.glsl", "shaders/fragmentShader_triangulation.glsl",
                "shaders/tessellationControlShader_quad.glsl", "shaders/tessellationEvaluationShader_quad.glsl", "shaders/geometryShader_triangulation.glsl");

            UpdateTessellationLevel(1);
        }

        void SetAmbientLight(float l) {
            glUseProgram(_shadowedProgram);
            glUniform1f(glGetUniformLocation(_shadowedProgram, "ambientLight"), l);
        }

        void SetShowTriangulation(bool t) {
            _showTriangulation = t;
        }

        GLFWwindow* GetWindow() {
            return _window;
        }

        void AddObject(Object3D* obj) {
            glBindVertexArray(_vao);
            GLuint VBO;
            glGenBuffers(1, &VBO);
            obj->SetVbo(VBO);

            _objects[_objectsCount] = obj;
            _objectsCount++;
        }
        
        void SetSpotLight(SpotLight3D* light) {
            _spotLight = light;
        }

        void SetMainCamera(Camera3D* camera) {
            _mainCamera = camera;
        }

        int RenderFrame() {
            double start = sg::getCurrentTimeMillis();

            sg::SetMatrix(glm::transpose(glm::inverse(glm::mat3(_mainCamera->GetView()))), _shadowedProgram, "mvt");
            sg::SetMatrix(glm::inverse(glm::mat3(_mainCamera->GetViewProjection())), _shadowedProgram, "matrixPV");
            sg::SetMatrix(glm::inverse(glm::mat3(_mainCamera->GetView())), _shadowedProgram, "matrixVinverse");
            sg::UpdateLightPos(_spotLight->GetLocalPosition(), _mainCamera->GetView(), _shadowedProgram);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _spotLight->GetShadowBuffer().bufferIndex);
            glClearColor(0, 0, 0, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, _spotLight->GetShadowWidth(), _spotLight->GetShadowHeight());

            for (int i = 0; i < _objectsCount; i++) {
                if (_objects[i]->CastsShadows)
                    _objects[i]->Draw(_spotLight->GetViewProjection(), _depthProgram);
            }

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _origFB);
            glViewport(0, 0, _width, _height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            sg::SetMatrix(_mainCamera->GetView(), _shadowedProgram, "mv");
            sg::SetMatrix(_spotLight->GetShadow(), _shadowedProgram, "shadowMatrix");
            for (int i = 0; i < _objectsCount; i++) {
                if (_objects[i]->Lit) {
                    if (_objects[i]->ReceivesShadows) {
                        _objects[i]->Draw(_mainCamera->GetViewProjection(), _shadowedProgram);
                    } else {
                        //shadowed unlit
                    }
                } else {
                    if (_objects[i]->ReceivesShadows) {
                        //not shadowed lit
                    }
                    else {
                        _objects[i]->Draw(_mainCamera->GetViewProjection(), _unlitProgram);
                    }
                }
            }

            if (_showTriangulation) {
                for (int i = 0; i < _objectsCount; i++) {
                    _objects[i]->Draw(_mainCamera->GetViewProjection(), _triangulationProgram);
                }
            }

            glfwSwapBuffers(_window);

            double elapsed = (sg::getCurrentTimeMillis() - start);
            double sleepTime = glm::max(0.0, (_timestep - elapsed));
            std::this_thread::sleep_for(std::chrono::milliseconds((long)sleepTime));

            return (int)(1000 / (sleepTime + elapsed)); //prima era long, non so se int va bene, testare
        }

        void IncreaseTessellationLevel() {
            _tessellationLevel++;
            if (_tessellationLevel > 64) _tessellationLevel = 64;
            UpdateTessellationLevel(_tessellationLevel);
        }

        void DecreaseTessellationLevel() {
            _tessellationLevel--;
            if (_tessellationLevel < 1) _tessellationLevel = 1;
            UpdateTessellationLevel(_tessellationLevel);
        }

        void SetNormalTexture(const char* filename) {
            sg::SetTexture(filename, _textureCount);
            glUniform1i(glGetUniformLocation(_shadowedProgram, "normalTexture"), _textureCount);
            _textureCount++;
        }

        void SetDisplacementTexture(const char* filename) {
            sg::SetTexture(filename, _textureCount);
            SetDisplacementUniforms(_textureCount);
            _textureCount++;
        }

        void SetupShadows(sg::SpotLight3D* light, int shadowResx, int shadowResy) {
            light->SetupShadows(shadowResx, shadowResy, _textureCount);
            glUseProgram(_shadowedProgram);
            glUniform1i(glGetUniformLocation(_shadowedProgram, "shadowTexture"), _textureCount);
            _textureCount++;
        }
	};
}