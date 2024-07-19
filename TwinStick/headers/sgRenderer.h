#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sgUtils.h>
#include <sgObject3D.h>
#include <sgSpotLight3D.h>
#include <sgDirectionalLight3D.h>
#include <sgAmbientLight.h>
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
        std::vector<SpotLight3D*> _spotLights;
        std::vector<DirectionalLight3D*> _directionalLights;
        std::vector<AmbientLight*> _ambientLights;
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
            for (int i = 0; i < _spotLights.size(); i++) {
                _spotLights[i]->Start();
            }
            for (int i = 0; i < _directionalLights.size(); i++) {
                _directionalLights[i]->Start();
            }
            for (int i = 0; i < _ambientLights.size(); i++) {
                _ambientLights[i]->Start();
            }
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
            for (int i = 0; i < _spotLights.size(); i++) {
                _spotLights[i]->Update(dt);
            }
            for (int i = 0; i < _directionalLights.size(); i++) {
                _directionalLights[i]->Update(dt);
            }
            for (int i = 0; i < _ambientLights.size(); i++) {
                _ambientLights[i]->Update(dt);
            }
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
            int textureUnit = 2;
            sg::UpdateSpotLights(_shadowedProgram, _spotLights, _mainCamera->GetView(), textureUnit);
            sg::UpdateSpotLights(_litProgram, _spotLights, _mainCamera->GetView(), textureUnit);

            textureUnit += _spotLights.size();
            sg::UpdateDirectionalLights(_shadowedProgram, _directionalLights, _mainCamera->GetView(), textureUnit);
            sg::UpdateDirectionalLights(_litProgram, _directionalLights, _mainCamera->GetView(), textureUnit);

            sg::UpdateAmbientLights(_shadowedProgram, _ambientLights);
            sg::UpdateAmbientLights(_litProgram, _ambientLights);
        }

        void RenderShadows() {
            glCullFace(GL_FRONT);

            for (int i = 0; i < _spotLights.size(); i++) {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _spotLights[i]->GetShadowBuffer().bufferIndex);
                glClearColor(0, 0, 0, 0);
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, _spotLights[i]->GetShadowWidth(), _spotLights[i]->GetShadowHeight());

                for (int j = 0; j < _objects.size(); j++) {
                    if (_objects[j]->CastsShadows) {
                        _objects[j]->Draw(_spotLights[i]->GetViewProjection(), _depthProgram);
                    }
                }
            }

            glCullFace(GL_BACK);

            for (int i = 0; i < _directionalLights.size(); i++) {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _directionalLights[i]->GetShadowBuffer().bufferIndex);
                glClearColor(0, 0, 0, 0);
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, _directionalLights[i]->GetShadowWidth(), _directionalLights[i]->GetShadowHeight());

                for (int j = 0; j < _objects.size(); j++) {
                    if (_objects[j]->CastsShadows) {
                        _objects[j]->Draw(_directionalLights[i]->GetViewProjection(), _depthProgram);
                    }
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
        
        void AddSpotLight(SpotLight3D* light) {
            _spotLights.push_back(light);
        }

        void AddDirectionalLight(DirectionalLight3D* light) {
            _directionalLights.push_back(light);
        }

        void AddAmbientLight(AmbientLight* light) {
            _ambientLights.push_back(light);
        }

        void RemoveEntity(Entity3D* ent) {
            int index = -1;
            for (int i = 0; i < _entities.size(); i++) {
                if (_entities[i] == ent) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _entities.erase(std::next(_entities.begin(), index));
            }
        }

        void RemoveObject(Object3D* obj) {
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

        void RemoveSpotLight(SpotLight3D* light) {
            int index = -1;
            for (int i = 0; i < _spotLights.size(); i++) {
                if (_spotLights[i] == light) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _spotLights.erase(std::next(_spotLights.begin(), index));
            }
        }

        void RemoveDirectionalLight(DirectionalLight3D* light) {
            int index = -1;
            for (int i = 0; i < _directionalLights.size(); i++) {
                if (_directionalLights[i] == light) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _directionalLights.erase(std::next(_directionalLights.begin(), index));
            }
        }

        void RemoveAmbientLight(AmbientLight* light) {
            int index = -1;
            for (int i = 0; i < _ambientLights.size(); i++) {
                if (_ambientLights[i] == light) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _ambientLights.erase(std::next(_ambientLights.begin(), index));
            }
        }

        void SetMainCamera(Camera3D* camera) {
            _mainCamera = camera;
        }

        void SetSkybox(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz) {
            const char* textureFaces[6] = { posx, negx, posy, negy, posz, negz };
            _skybox.InitSkybox(textureFaces, _vao);
        }

        void RemoveAllEntities() {
            while (_entities.size() > 0)
                _entities.erase(_entities.begin());
            while (_objects.size() > 0)
                _objects.erase(_objects.begin());
            while (_spotLights.size() > 0)
                _spotLights.erase(_spotLights.begin());
            while (_directionalLights.size() > 0)
                _directionalLights.erase(_directionalLights.begin());
            while (_ambientLights.size() > 0)
                _ambientLights.erase(_ambientLights.begin());
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
                        for (int j = 0; j < _spotLights.size(); j++) {
                            std::string str = std::string("spotShadowMatrices[").append(std::to_string(j)).append("]");
                            sg::SetMatrix(_spotLights[j]->GetShadow() * _objects[i]->GetModelMatrix(), _shadowedProgram, str.c_str());
                        }
                        for (int j = 0; j < _directionalLights.size(); j++) {
                            std::string str = std::string("dirShadowMatrices[").append(std::to_string(j)).append("]");
                            sg::SetMatrix(_directionalLights[j]->GetShadow() * _objects[i]->GetModelMatrix(), _shadowedProgram, str.c_str());
                        }

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