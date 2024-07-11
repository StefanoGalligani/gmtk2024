#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sgUtils.h>
#include <sgObject3D.h>
#include <sgSpotLight3D.h>
#include <sgCamera3D.h>
#include <sgSkyboxRenderer.h>
#include <thread>

namespace sg {
	class Renderer {
    private:
        GLuint _shadowedProgram;
        GLuint _depthProgram;
        GLuint _unlitProgram;
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
        std::vector<Object3D*> _objects;
        std::vector<Entity3D*> _entities;

        double _timestep = 1000.0 / 40;
        int _tessellationLevel = 1;
        bool _firstFrame = true;
        double _lastDt;

    public:
        int InitWindow(const char* title, int width, int height) {
            /* Initialize the library */
            if (!glfwInit())
                return -1;

            /* Create a windowed mode window and its OpenGL context */
            glfwWindowHint(GLFW_SAMPLES, 4);
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
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_CULL_FACE);

            return 0;
        }

        void InitPrograms() {
            _shadowedProgram = sg::CreateProgram("shaders/vertexShader_shadowed.glsl", "shaders/fragmentShader_shadowed.glsl");
            _depthProgram = sg::CreateProgram("shaders/vertexShader_depth.glsl", "shaders/fragmentShader_depth.glsl");
            _unlitProgram = sg::CreateProgram("shaders/vertexShader_unlit.glsl", "shaders/fragmentShader_unlit.glsl");
            _triangulationProgram = sg::CreateProgram("shaders/vertexShader_triangulation.glsl", "shaders/fragmentShader_triangulation.glsl", "shaders/geometryShader_triangulation.glsl");
        }

        void SetResolution(int x, int y) {
            _width = x;
            _height = y;
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
        
        void SetSpotLight(SpotLight3D* light) {
            _spotLight = light;
        }

        void SetMainCamera(Camera3D* camera) {
            _mainCamera = camera;
        }

        void StartAll() {
            for (int i = 0; i < _entities.size(); i++) {
                _entities[i]->Start();
            }
            for (int i = 0; i < _objects.size(); i++) {
                _objects[i]->Start();
            }
            _spotLight->Start();
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
            _mainCamera->Update(dt);
        }

        void UpdateOrStart() {
            if (!_firstFrame) {
                UpdateAll(_lastDt);
            } else {
                StartAll();
                _firstFrame = false;
            }
        }

        int RenderFrame() {
            double start = sg::getCurrentTimeMillis();

            UpdateOrStart();

            sg::SetMatrix(glm::transpose(glm::inverse(glm::mat3(_mainCamera->GetView()))), _shadowedProgram, "mvt");
            sg::SetMatrix(glm::inverse(glm::mat3(_mainCamera->GetViewProjection())), _shadowedProgram, "matrixPV");
            sg::SetMatrix(glm::inverse(glm::mat3(_mainCamera->GetView())), _shadowedProgram, "matrixVinverse");
            sg::UpdateLightPos(_spotLight->GetGlobalPosition(), _mainCamera->GetView(), _shadowedProgram);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _spotLight->GetShadowBuffer().bufferIndex);
            glClearColor(0, 0, 0, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, _spotLight->GetShadowWidth(), _spotLight->GetShadowHeight());

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->CastsShadows) {
                    _objects[i]->Draw(_spotLight->GetViewProjection(), _depthProgram);
                }
            }

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _origFB);
            glViewport(0, 0, _width, _height);
            glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);

            for (int i = 0; i < _objects.size(); i++) {
                if (_objects[i]->Lit) {
                    if (_objects[i]->ReceivesShadows) {
                        sg::SetMatrix(_mainCamera->GetView() * _objects[i]->GetModelMatrix(), _shadowedProgram, "mv");
                        sg::SetMatrix(_spotLight->GetShadow() * _objects[i]->GetModelMatrix(), _shadowedProgram, "shadowMatrix");
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

        void SetupShadows(sg::SpotLight3D* light, int shadowResx, int shadowResy) {
            light->SetupShadows(shadowResx, shadowResy, 5); //il 5 è arbitrario, poi andrà fatto gestire al TextureManager
            glUseProgram(_shadowedProgram);
            glUniform1i(glGetUniformLocation(_shadowedProgram, "shadowTexture"), 5);
        }

        void SetSkybox(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz) {
            const char* textureFaces[6] = { posx, negx, posy, negy, posz, negz };
            _skybox.InitSkybox(textureFaces, _vao);
        }
	};
}