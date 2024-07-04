#include <sgEngine.h>
#include <Board.h>

sg::Renderer renderer = sg::Renderer();
sg::Object3D lightObj = sg::Object3D();
sg::Camera3D mainCamera = sg::Camera3D();
sg::SpotLight3D spotLight = sg::SpotLight3D();
Board board = Board();

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
            if (renderer.InitWindow("Quarto", resx, resy) < 0) return false;
            renderer.InitPrograms();
            BindInputs();
            InitObjects();

            renderer.SetAmbientLight(0.5f);
            renderer.SetupShadows(&spotLight, shadowResx, shadowResy);
            renderer.SetSkybox("res/skybox/posx.png", "res/skybox/negx.png", "res/skybox/posy.png",
                "res/skybox/negy.png", "res/skybox/posz.png", "res/skybox/negz.png");
            return true;
        }

        void mainLoop() {
            printf("Starting rendering\n");
            while (!glfwWindowShouldClose(renderer.GetWindow()))
            {
                renderer.RenderFrame();
                glfwPollEvents();
            }
        }

        void cleanup() {
            printf("Terminating");
            glfwTerminate();
        }

        void InitObjects() {
            printf("Initializing objects\n");

            lightObj.LoadModelFromObj("res/light/light.obj");
            lightObj.SetLocalPosition(0, 30, 60);

            mainCamera.SetPerspective(1.5f, (float)resx / resy, 0.05f, 3000.0f);
            mainCamera.SetLocalPosition(glm::vec3(0, 30, 55));
            mainCamera.LookAt(glm::vec3(0, 0, 0));

            spotLight.SetPerspective(1.0f, (float)shadowResx / shadowResy, 1.0f, 1000.0f);
            spotLight.SetLocalPosition(lightObj.GetLocalPosition());
            spotLight.LookAt(glm::vec3(0, 0, 0));

            renderer.AddObject(&lightObj);
            renderer.SetMainCamera(&mainCamera);
            renderer.SetSpotLight(&spotLight);

            board.Init(&renderer);

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
            glfwDestroyWindow(renderer.GetWindow());
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
                    lightObj.RotateAround(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), movex);
                    lightObj.RotateAround(-lightObj.Right(), glm::vec3(0, 0, 0), movey);
                    spotLight.SetLocalPosition(lightObj.GetLocalPosition());
                    spotLight.LookAt(glm::vec3(0, 0, 0));
                }
                if (pressedR) {
                    float movey = (ypos - prevy) / resy;
                    lightObj.Translate(lightObj.GetLocalPosition() * movey);
                    spotLight.SetLocalPosition(lightObj.GetLocalPosition());
                    spotLight.LookAt(glm::vec3(0, 0, 0));
                }
            }
            else {
                if (pressedL) {
                    double movex = -(xpos - prevx) / resx;
                    double movey = (ypos - prevy) / resy;
                    mainCamera.RotateAround(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), movex);
                    mainCamera.RotateAround(-mainCamera.Right(), glm::vec3(0, 0, 0), movey);
                    mainCamera.LookAt(glm::vec3(0, 0, 0));
                }
                if (pressedR) {
                    float movey = (ypos - prevy) / resy;
                    mainCamera.Translate(mainCamera.GetLocalPosition() * movey);
                    mainCamera.LookAt(glm::vec3(0, 0, 0));
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