#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sgUtils.h>
#include <sgObject3D.h>
#include <sgSpotLight3D.h>
#include <sgDirectionalLight3D.h>
#include <sgAmbientLight.h>
#include <sgPointLight3D.h>
#include <sgCamera3D.h>
#include <sgSkyboxRenderer.h>
#include <thread>

namespace sg {
	class Renderer {
    private:
        GLuint _shadowedProgram;
        GLuint _depthProgram;
        GLuint _depthLinearProgram;
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
        std::vector<PointLight3D*> _pointLights;
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
            sg::UpdateDirectionalLights(_shadowedProgram, _directionalLights, _mainCamera->GetView(), textureUnit);
            sg::UpdateDirectionalLights(_litProgram, _directionalLights, _mainCamera->GetView(), textureUnit);

            textureUnit += _directionalLights.size();
            sg::UpdatePointLights(_shadowedProgram, _pointLights, _mainCamera->GetView(), textureUnit);
            sg::UpdatePointLights(_litProgram, _pointLights, _mainCamera->GetView(), textureUnit);

            textureUnit += _pointLights.size();
            sg::UpdateSpotLights(_shadowedProgram, _spotLights, _mainCamera->GetView(), textureUnit);
            sg::UpdateSpotLights(_litProgram, _spotLights, _mainCamera->GetView(), textureUnit);

            sg::UpdateAmbientLights(_shadowedProgram, _ambientLights);
            sg::UpdateAmbientLights(_litProgram, _ambientLights);
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

        void RemovePointLight(PointLight3D* light) {
            int index = -1;
            for (int i = 0; i < _pointLights.size(); i++) {
                if (_pointLights[i] == light) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                _pointLights.erase(std::next(_pointLights.begin(), index));
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

        void RenderShadows() {
            glUseProgram(_depthProgram);

            for (int i = 0; i < _spotLights.size(); i++) {
                if (!_spotLights[i]->FrustumCheck(_mainCamera->GetFrustum())) continue;
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _spotLights[i]->GetShadowBuffer().bufferIndex);
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, _spotLights[i]->GetShadowWidth(), _spotLights[i]->GetShadowHeight());

                for (int j = 0; j < _objects.size(); j++) {
                    if (_objects[j]->CastsShadows) {
                        _objects[j]->Draw(_depthProgram, _spotLights[i]->GetViewProjection(), _spotLights[i]->GetFrustum());
                    }
                }
            }

            for (int i = 0; i < _directionalLights.size(); i++) {
                if (!_directionalLights[i]->FrustumCheck(_mainCamera->GetFrustum())) continue;
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _directionalLights[i]->GetShadowBuffer().bufferIndex);
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, _directionalLights[i]->GetShadowWidth(), _directionalLights[i]->GetShadowHeight());

                for (int j = 0; j < _objects.size(); j++) {
                    if (_objects[j]->CastsShadows) {
                        _objects[j]->Draw(_depthProgram, _directionalLights[i]->GetViewProjection(), _directionalLights[i]->GetFrustum());
                    }
                }
            }

            glUseProgram(_depthLinearProgram);

            for (int i = 0; i < _pointLights.size(); i++) {
                if (!_pointLights[i]->FrustumCheck(_mainCamera->GetFrustum())) continue;
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pointLights[i]->GetShadowBuffer().bufferIndex);
                for (int face = 0; face < 6; face++) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, _pointLights[i]->GetShadowTexture(), 0);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    glViewport(0, 0, _pointLights[i]->GetShadowWidth(), _pointLights[i]->GetShadowHeight());

                    glUniform3fv(glGetUniformLocation(_depthLinearProgram, "lightPos"), 1, glm::value_ptr(_pointLights[i]->GetGlobalPosition()));
                    glUniform1f(glGetUniformLocation(_depthLinearProgram, "far_plane"), _pointLights[i]->GetFarPlane());

                    for (int j = 0; j < _objects.size(); j++) {
                        if (_objects[j]->CastsShadows) {
                            glUniformMatrix4fv(glGetUniformLocation(_depthLinearProgram, "model"), 1, false, glm::value_ptr(_objects[j]->GetModelMatrix()));
                            _objects[j]->Draw(_depthLinearProgram, _pointLights[i]->GetViewProjection(face), _pointLights[i]->GetFrustum(face));
                        }
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
            _depthLinearProgram = sg::CreateProgram("shaders/vertexShader_depth_linear.glsl", "shaders/fragmentShader_depth_linear.glsl");
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

        void AddLight(Light* light) {
            switch (light->GetLightType()) {
            case TypeAmbientLight:
                _ambientLights.push_back(static_cast<AmbientLight*>(light));
                break;
            case TypeDirectionalLight:
                _directionalLights.push_back(static_cast<DirectionalLight3D*>(light));
                break;
            case TypeSpotLight:
                _spotLights.push_back(static_cast<SpotLight3D*>(light));
                break;
            case TypePointLight:
                _pointLights.push_back(static_cast<PointLight3D*>(light));
                break;
            default:
                printf("Error: tried to add light of unspecified type\n");
                break;
            }
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

        void RemoveLight(Light* light) {
            switch (light->GetLightType()) {
            case TypeAmbientLight:
                RemoveAmbientLight(static_cast<AmbientLight*>(light));
                break;
            case TypeDirectionalLight:
                RemoveDirectionalLight(static_cast<DirectionalLight3D*>(light));
                break;
            case TypeSpotLight:
                RemoveSpotLight(static_cast<SpotLight3D*>(light));
                break;
            case TypePointLight:
                RemovePointLight(static_cast<PointLight3D*>(light));
                break;
            default:
                printf("Error: tried to remove light of unspecified type\n");
                return;
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
            while (_pointLights.size() > 0)
                _pointLights.erase(_pointLights.begin());
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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->Lit) {
                    GLuint program = _objects[i]->ReceivesShadows ? _shadowedProgram : _litProgram;
                    glm::mat4 mv = _mainCamera->GetView() * _objects[i]->GetModelMatrix();
                    sg::SetMatrix(mv, program, "mv");
                    sg::SetMatrix(_objects[i]->GetModelMatrix(), program, "modelMat");
                    sg::SetMatrix(glm::transpose(glm::inverse(glm::mat3(mv))), program, "mvt");
                    for (int j = 0; j < _spotLights.size(); j++) {
                        std::string str = std::string("spotShadowMatrices[").append(std::to_string(j)).append("]");
                        sg::SetMatrix(_spotLights[j]->GetShadow() * _objects[i]->GetModelMatrix(), program, str.c_str());
                    }
                    for (int j = 0; j < _directionalLights.size(); j++) {
                        std::string str = std::string("dirShadowMatrices[").append(std::to_string(j)).append("]");
                        sg::SetMatrix(_directionalLights[j]->GetShadow() * _objects[i]->GetModelMatrix(), program, str.c_str());
                    }

                    _objects[i]->Draw(program, _mainCamera->GetViewProjection(), _mainCamera->GetFrustum());
                } else {
                    _objects[i]->Draw(_unlitProgram, _mainCamera->GetViewProjection(), _mainCamera->GetFrustum());
                }
            }

            if (_showTriangulation) {
                for (int i = 0; i < _objects.size(); i++) {
                    _objects[i]->Draw(_triangulationProgram, _mainCamera->GetViewProjection(), _mainCamera->GetFrustum());
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