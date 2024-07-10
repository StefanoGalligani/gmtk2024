#include <sgEngine.h>
#include <sstream>

sg::Renderer renderer = sg::Renderer();
sg::Object3D lightObj = sg::Object3D();
sg::Camera3D mainCamera = sg::Camera3D();
sg::SpotLight3D spotLight = sg::SpotLight3D();
sg::Object3D playerObj = sg::Object3D();
sg::Object3D zombieObj = sg::Object3D();
sg::Object3D mapObj = sg::Object3D();
sg::Object3D shedObj = sg::Object3D();
sg::Object3D siloObj = sg::Object3D();
sg::Object3D siloObj2 = sg::Object3D();
sg::Object3D treeObj = sg::Object3D();

bool showTriangulation = false;
bool pressedCTRL = false;
int pressedL = 0;
int pressedR = 0;
double prevx = 0;
double prevy = 0;

float resx = 1080;
float resy = 720;
float shadowResx = 2048;
float shadowResy = 2048;

namespace sg {
    class sgGame {
    public:
        void run() {
            if (!initApplication()) return;
            mainLoop();
            cleanup();
        }

    private:
        bool initApplication() {
            if (renderer.InitWindow("TwinStick", resx, resy) < 0) return false;
            renderer.InitPrograms();
            BindInputs();
            InitObjects();

            renderer.SetAmbientLight(0.3f);
            renderer.SetupShadows(&spotLight, shadowResx, shadowResy);
            renderer.SetSkybox("res/skybox/posx.png", "res/skybox/negx.png", "res/skybox/posy.png",
                "res/skybox/negy.png", "res/skybox/posz.png", "res/skybox/negz.png");
            return true;
        }

        void mainLoop() {
            printf("Starting rendering\n");
            while (!renderer.Terminated())
            {
                int fps = renderer.RenderFrame();
                std::stringstream ss{};
                ss << "TwinStick [" << fps << " FPS]";
                glfwSetWindowTitle(renderer.GetWindow(), ss.str().c_str());

                glfwPollEvents();
            }
        }

        void cleanup() {
            printf("Terminating");
            glfwTerminate();
        }

        void InitObjects() {
            printf("Initializing objects\n");

            playerObj.LoadModelFromObj("res/models/player.obj");
            playerObj.Lit = true;
            playerObj.CastsShadows = true;
            playerObj.ReceivesShadows = true;
            playerObj.PerformFrustumCheck = false;
            playerObj.SetGlobalPosition(0, 0, -1.5);

            zombieObj.LoadModelFromObj("res/models/zombie.obj");
            zombieObj.Lit = true;
            zombieObj.CastsShadows = true;
            zombieObj.ReceivesShadows = true;
            zombieObj.SetGlobalPosition(0, 0, 1.5);
            zombieObj.RotateGlobal(glm::vec3(0,1,0), 3.1415926535f);

            mapObj.LoadModelFromObj("res/models/map.obj");
            mapObj.Lit = true;
            mapObj.ReceivesShadows = true;
            mapObj.PerformFrustumCheck = false;
            shedObj.LoadModelFromObj("res/models/shed.obj");
            shedObj.Lit = true;
            shedObj.CastsShadows = true;
            shedObj.ReceivesShadows = true;
            siloObj.LoadModelFromObj("res/models/silo.obj");
            siloObj.Lit = true;
            siloObj.CastsShadows = true;
            siloObj.ReceivesShadows = true;
            siloObj2.SetModel(siloObj.GetModel());
            siloObj2.Lit = true;
            siloObj2.CastsShadows = true;
            siloObj2.ReceivesShadows = true;
            siloObj2.SetGlobalPosition(0, 0, 15);
            treeObj.LoadModelFromObj("res/models/tree.obj");
            treeObj.Lit = true;
            treeObj.CastsShadows = true;
            treeObj.ReceivesShadows = true;

            lightObj.SetGlobalPosition(0, 5, 8);

            mainCamera.SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
            mainCamera.SetGlobalPosition(glm::vec3(0, 5, 7));
            mainCamera.LookAtGlobal(glm::vec3(0, 0, 0));

            spotLight.SetPerspective(1.0f, (float)shadowResx / shadowResy, 1.0f, 1000.0f);
            spotLight.SetGlobalPosition(lightObj.GetGlobalPosition());
            spotLight.LookAtGlobal(glm::vec3(0, 0, 0));

            renderer.AddObject(&playerObj);
            renderer.AddObject(&zombieObj);
            renderer.AddObject(&mapObj);
            renderer.AddObject(&shedObj);
            renderer.AddObject(&siloObj);
            renderer.AddObject(&siloObj2);
            renderer.AddObject(&treeObj);
            renderer.SetMainCamera(&mainCamera);
            renderer.SetSpotLight(&spotLight);

            printf("Buffers ready\n");
        }

        void BindInputs() {
            BindInput(sg::Key_Esc_Down, onEscKeyPressed);
            BindInput(sg::Key_Space_Down, onSpaceKeyPressed);
            BindInput(sg::Mouse_Left_Down, onLeftMouseButtonClick);
            BindInput(sg::Mouse_Left_Up, onLeftMouseButtonRelease);
            BindInput(sg::Mouse_Right_Down, onRightMouseButtonClick);
            BindInput(sg::Mouse_Right_Up, onRightMouseButtonRelease);
            BindInput(sg::Mouse_Position, onMouseDrag);
            BindInput(sg::Window_Resize, onWindowResize);
        }

        void BindInput(int cmd, sgKeyOrMouseFun callback) {
            sg::InputManager::Instance()->BindInput(renderer.GetWindow(), cmd, callback);
        }
        void BindInput(int cmd, sgCursorPosFun callback) {
            sg::InputManager::Instance()->BindInput(renderer.GetWindow(), cmd, callback);
        }
        void BindInput(int cmd, sgWindowSizeFun callback) {
            sg::InputManager::Instance()->BindInput(renderer.GetWindow(), cmd, callback);
        }

        static void onEscKeyPressed(int mods) {
            renderer.DestroyWindow();
        }

        static void onSpaceKeyPressed(int mods) {
            showTriangulation = !showTriangulation;
            renderer.SetShowTriangulation(showTriangulation);
        }

        static void onWindowResize(int x, int y) {
            resx = x;
            resy = y;
            renderer.SetResolution(resx, resy);
            mainCamera.SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
        }

        static void onLeftMouseButtonClick(int mods) {
            pressedL = true;
            pressedCTRL = (mods & GLFW_MOD_CONTROL) > 0;
        }

        static void onLeftMouseButtonRelease(int mods) {
            pressedL = false;
        }

        static void onRightMouseButtonClick(int mods) {
            pressedR = true;
            pressedCTRL = (mods & GLFW_MOD_CONTROL) > 0;
        }

        static void onRightMouseButtonRelease(int mods) {
            pressedR = false;
        }

        static void onMouseDrag(double xpos, double ypos) {
            if (pressedCTRL) {
                if (pressedL) {
                    double movex = (xpos - prevx) / resx;
                    double movey = -(ypos - prevy) / resy;
                    lightObj.RotateAroundGlobal(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), movex);
                    lightObj.RotateAroundGlobal(-lightObj.GlobalRight(), glm::vec3(0, 0, 0), movey);
                    spotLight.SetGlobalPosition(lightObj.GetGlobalPosition());
                    spotLight.LookAtGlobal(glm::vec3(0, 0, 0));
                }
                if (pressedR) {
                    float movey = (ypos - prevy) / resy;
                    lightObj.TranslateGlobal(lightObj.GetGlobalPosition() * movey);
                    spotLight.SetGlobalPosition(lightObj.GetGlobalPosition());
                    spotLight.LookAtGlobal(glm::vec3(0, 0, 0));
                }
            }
            else {
                if (pressedL) {
                    double movex = -(xpos - prevx) / resx;
                    mainCamera.RotateAroundGlobal(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), movex);

                    double movey = (ypos - prevy) / resy;
                    float yForward = mainCamera.GlobalForward().y;
                    if (glm::abs(yForward) < 0.99 || yForward * movey > 0) {
                        mainCamera.RotateAroundGlobal(-mainCamera.GlobalRight(), glm::vec3(0, 0, 0), movey);
                    }
                }
                if (pressedR) {
                    float movey = (ypos - prevy) / resy;
                    mainCamera.TranslateGlobal(mainCamera.GetGlobalPosition() * movey);
                }
            }
            prevx = xpos;
            prevy = ypos;
        }
    };
}

int main(int argc, char* argv[]) {
    sg::sgGame game;

    try {
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}