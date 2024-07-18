#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sgUtils.h>
#include <sgObject3D.h>
#include <sgSpotLight3D.h>
#include <sgDirectionalLight3D.h>
#include <sgCamera3D.h>
#include <sgSkyboxRenderer.h>
#include <thread>

namespace sg {
	class Renderer {
    private:
        GLuint _shadowedProgram;
        GLuint _depthProgram;
        GLuint _unlitProgram;
        GLuint _litProgram;
        GLuint _triangulationProgram;
        bool _showTriangulation;
        SkyboxRenderer _skybox;

        GLFWwindow* _window;
        GLint _origFB;
        int _width;
        int _height;
        GLuint _vao;

        Camera3D* _mainCamera;
        SpotLight3D* _spotLight;
        DirectionalLight3D* _directionalLight;
        std::vector<Object3D*> _objects;
        std::vector<Entity3D*> _entities;

        double _timestep = 1000.0 / 40;
        int _tessellationLevel = 1;
        bool _firstFrame = true;
        double _lastDt;

        void StartAll() {
            for (int i = 0; i < _entities.size(); i++) {
                _entities[i]->Start();
            }
            for (int i = 0; i < _objects.size(); i++) {
                _objects[i]->Start();
            }
            _spotLight->Start();
            _directionalLight->Start();
            _mainCamera->Start();
        }

        void UpdateAll(double dt) {
            dt /= 1000;
            for (int i = 0; i < _entities.size(); i++) {
                _entities[i]->Update(dt);
            }
            for (int i = 0; i < _objects.size(); i++) {
                _objects[i]->Update(dt);
            }
            _spotLight->Update(dt);
            _directionalLight->Update(dt);
            _mainCamera->Update(dt);
        }

        void UpdateOrStart() {
            if (!_firstFrame) {
                UpdateAll(_lastDt);
            }
            else {
                StartAll();
                _firstFrame = false;
            }
        }

        void UpdateLights() {
            sg::UpdateSpotLight(_spotLight, _mainCamera->GetView(), _shadowedProgram);
            sg::UpdateDirectionalLight(_directionalLight, _shadowedProgram);
            sg::UpdateSpotLight(_spotLight, _mainCamera->GetView(), _litProgram);
            sg::UpdateDirectionalLight(_directionalLight, _litProgram);
        }

        void RenderShadows() {
            glCullFace(GL_FRONT);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _spotLight->GetShadowBuffer().bufferIndex);
            glClearColor(0, 0, 0, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, _spotLight->GetShadowWidth(), _spotLight->GetShadowHeight());

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->CastsShadows) {
                    _objects[i]->Draw(_spotLight->GetViewProjection(), _depthProgram);
                }
            }

            glCullFace(GL_BACK);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _directionalLight->GetShadowBuffer().bufferIndex);
            glClearColor(0, 0, 0, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, _directionalLight->GetShadowWidth(), _directionalLight->GetShadowHeight());

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->CastsShadows) {
                    _objects[i]->Draw(_directionalLight->GetViewProjection(), _depthProgram);
                }
            }
        }

    public:
        int InitRenderer(GLFWwindow* window, int width, int height) {
            _window = window;
            _width = width;
            _height = height;

            glClearColor(0, 0, 0, 0);
            glViewport(0, 0, width, height);

            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &_origFB);

            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glPatchParameteri(GL_PATCH_VERTICES, 4);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_CULL_FACE);

            _shadowedProgram = sg::CreateProgram("shaders/vertexShader_shadowed.glsl", "shaders/fragmentShader_shadowed.glsl");
            _depthProgram = sg::CreateProgram("shaders/vertexShader_depth.glsl", "shaders/fragmentShader_depth.glsl");
            _unlitProgram = sg::CreateProgram("shaders/vertexShader_unlit.glsl", "shaders/fragmentShader_unlit.glsl");
            _litProgram = sg::CreateProgram("shaders/vertexShader_lit.glsl", "shaders/fragmentShader_lit.glsl");
            _triangulationProgram = sg::CreateProgram("shaders/vertexShader_triangulation.glsl", "shaders/fragmentShader_triangulation.glsl", "shaders/geometryShader_triangulation.glsl");

            return 0;
        }

        void SetResolution(int x, int y) {
            _width = x;
            _height = y;
        }

        void SetAmbientLight(float l) {
            glUseProgram(_shadowedProgram);
            glUniform1f(glGetUniformLocation(_shadowedProgram, "ambientLight"), l);
            glUseProgram(_litProgram);
            glUniform1f(glGetUniformLocation(_litProgram, "ambientLight"), l);
        }

        void SetShowTriangulation(bool t) {
            _showTriangulation = t;
        }

        GLFWwindow* GetWindow() {
            return _window;
        }

        void DestroyWindow() {
            if (_window != NULL) {
                glfwDestroyWindow(_window);
                _window = NULL;
            }
        }

        bool Terminated() {
            return _window == NULL || glfwWindowShouldClose(_window);
        }

        void AddEntity(Entity3D* entity) {
            _entities.push_back(entity);
        }

        void AddObject(Object3D* obj) {
            obj->GetModel()->SetVBO(_vao);
            _objects.push_back(obj);
        }

        void RemoveObject(Object3D *obj) {
            int index = -1;
            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i] == obj) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _objects.erase(std::next(_objects.begin(), index));
            }
        }
        
        void SetSpotLight(SpotLight3D* light) {
            _spotLight = light;
        }

        void SetDirectionalLight(DirectionalLight3D* light) {
            _directionalLight = light;
        }

        void SetMainCamera(Camera3D* camera) {
            _mainCamera = camera;
        }

        void SetSkybox(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz) {
            const char* textureFaces[6] = { posx, negx, posy, negy, posz, negz };
            _skybox.InitSkybox(textureFaces, _vao);
        }

        void RemoveAllEntities() {
            while (_objects.size() > 0)
                _objects.erase(_objects.begin());
            while (_entities.size() > 0)
                _entities.erase(_entities.begin());
        }

        int RenderFrame() {
            double start = sg::getCurrentTimeMillis();

            UpdateOrStart();
            UpdateLights();

            RenderShadows();

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _origFB);
            glViewport(0, 0, _width, _height);
            glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->Lit) {
                    glm::mat4 mv = _mainCamera->GetView() * _objects[i]->GetModelMatrix();
                    if (_objects[i]->ReceivesShadows) {
                        sg::SetMatrix(mv, _shadowedProgram, "mv");
                        sg::SetMatrix(glm::transpose(glm::inverse(glm::mat3(mv))), _shadowedProgram, "mvt");
                        sg::SetMatrix(_spotLight->GetShadow() * _objects[i]->GetModelMatrix(), _shadowedProgram, "spotShadowMatrix");
                        sg::SetMatrix(_directionalLight->GetShadow() * _objects[i]->GetModelMatrix(), _shadowedProgram, "dirShadowMatrix");
                        _objects[i]->Draw(_mainCamera->GetViewProjection(), _shadowedProgram);
                    } else {
                        sg::SetMatrix(mv, _litProgram, "mv");
                        sg::SetMatrix(glm::transpose(glm::inverse(glm::mat3(mv))), _litProgram, "mvt");
                        _objects[i]->Draw(_mainCamera->GetViewProjection(), _litProgram);
                    }
                } else {
                    _objects[i]->Draw(_mainCamera->GetViewProjection(), _unlitProgram);
                }
            }

            if (_showTriangulation) {
                for (int i = 0; i < _objects.size(); i++) {
                    _objects[i]->Draw(_mainCamera->GetViewProjection(), _triangulationProgram);
                }
            }

            if (_skybox.IsPresent()) {
                _skybox.RenderSkybox(_mainCamera);
            }

            glfwSwapBuffers(_window);

            double elapsed = (sg::getCurrentTimeMillis() - start) / 1000;
            double sleepTime = glm::max(0.0, (_timestep - elapsed));
            std::this_thread::sleep_for(std::chrono::milliseconds((long)sleepTime));

            _lastDt = (sg::getCurrentTimeMillis() - start) / 1000;

            return (int)(1000 / _lastDt);
        }
	};
}