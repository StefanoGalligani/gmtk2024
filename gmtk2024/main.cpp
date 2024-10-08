#include <sgEngine.h>
#include <sstream>
#include <Player.h>
#include <EnemyManager.h>
#include <MapCreator.h>
#include <irrKlang.h>
using namespace irrklang;

sg::Renderer* renderer;
Player* player;
EnemyManager* enemyManager;
MapCreator* mapCreator;
sg::Model* bulletModel;
sg::AmbientLight* ambientLight;

bool minimized = false;

float resx = 1080;
float resy = 720;
float prevx = -1;
float prevy = -1;
int shadowResx = 1024;
int shadowResy = 1024;
bool restart = false;
bool endScreenShown = false;

ISoundEngine* SoundEngine = createIrrKlangDevice();
ISound* engineSound = NULL;

#define PLAYER_HEALTH 5
#define PLAYER_ACCELERATION 15
#define PLAYER_DECELERATION 4
#define PLAYER_MAX_VELOCITY 40
#define ENEMY_SPEED 4
#define BULLET_SPEED 50
#define BULLET_LIFETIME 1
#define XROTSPEED 10
#define YROTSPEED 5

class sgGame {
public:
    void run() {
        if (!initWindow()) return;
        SoundEngine->setSoundVolume(0);
        SoundEngine->play2D("res/sfx/hit.wav");
        SoundEngine->play2D("res/sfx/laser.wav");
        SoundEngine->play2D("res/sfx/engine.wav");
        BindInputs();

        initGame();
        mainLoop();
        cleanup();
        closeApplication();
    }

private:
    bool initWindow() {
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_SAMPLES, 4);
        GLFWwindow* window = glfwCreateWindow(resx, resy, "Antibiotic", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK) {
            std::cout << "Error with Glew" << std::endl;
            return -1;
        }
        std::cout << glGetString(GL_VERSION) << std::endl;

        renderer = new sg::Renderer();
        if (renderer->InitRenderer(window, resx, resy) < 0) return false;

        glfwSetCursorPos(renderer->GetWindow(), resx / 2, resy / 2);
        prevx = resx / 2;
        prevy = resy / 2;
        return true;
    }

    void initGame() {
        InitObjects();
        glfwSetInputMode(renderer->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void InitObjects() {
        printf("Initializing objects\n");
        mapCreator = new MapCreator(renderer);
        player = new Player(renderer, PLAYER_HEALTH, 0.05f, PLAYER_ACCELERATION, PLAYER_DECELERATION, PLAYER_MAX_VELOCITY, shadowResx, shadowResy, resx, resy, mapCreator);
        enemyManager = new EnemyManager(renderer, player, SoundEngine);

        ambientLight = new sg::AmbientLight(1.0f);
        renderer->AddLight(ambientLight);
    }

    int averageFrameRate(int newFrameRate) {
        static std::vector<int> frameRates = std::vector<int>();
        static int sum = 0;
        sum += newFrameRate;
        frameRates.push_back(newFrameRate);
        if (frameRates.size() > 30) {
            sum -= frameRates[0];
            frameRates.erase(frameRates.begin());
        }
        return sum / frameRates.size();
    }

    void mainLoop() {
        printf("Starting rendering\n");
        while (!renderer->Terminated())
        {
            if (!minimized) {
                if (checkGameOver()) {
                    cleanup();
                    initGame();
                }
                if (enemyManager->finished && !endScreenShown) {
                    endScreenShown = true;
                    renderer->RemoveAllEntities();
                    renderer->AddEndScreen();
                }

                int fps = renderer->RenderFrame();
                std::stringstream ss{};
                ss << "Antibiotic [" << averageFrameRate(fps) << " FPS]";
                glfwSetWindowTitle(renderer->GetWindow(), ss.str().c_str());
            }
            glfwPollEvents();
        }
    }

    bool checkGameOver() {
        if (restart) {
            restart = false;
            return true;
        }
        return player->IsDead();
    }

    void cleanup() {
        printf("Terminating\n");
        delete(player);
        delete(enemyManager);
        delete(bulletModel);
        delete(mapCreator);
        delete(ambientLight);
        renderer->RemoveAllEntities();
        if (engineSound != NULL) engineSound->setIsPaused(true);
        endScreenShown = false;
    }

    void closeApplication() {
        renderer->DestroyWindow();
        delete(renderer);
        glfwTerminate();
    }

#pragma region input
    void BindInputs() {
        BindInput(sg::Key_Esc_Down, onEscKeyPressed);
        BindInput(sg::Mouse_Left_Down, onLeftMouseButtonClick);
        BindInput(sg::Mouse_Position, onMouseDrag);
        BindInput(sg::Window_Resize, onWindowResize);

        BindInput(sg::Key_W_Down, onWPressed);
        BindInput(sg::Key_W_Up, onWReleased);
        BindInput(sg::Key_R_Down, onRPressed);
    }

    void BindInput(int cmd, sg::sgKeyOrMouseFun callback) {
        sg::InputManager::Instance()->BindInput(renderer->GetWindow(), cmd, callback);
    }
    void BindInput(int cmd, sg::sgCursorPosFun callback) {
        sg::InputManager::Instance()->BindInput(renderer->GetWindow(), cmd, callback);
    }
    void BindInput(int cmd, sg::sgWindowSizeFun callback) {
        sg::InputManager::Instance()->BindInput(renderer->GetWindow(), cmd, callback);
    }

    static void onEscKeyPressed(int mods) {
        renderer->DestroyWindow();
    }

    static void onWindowResize(int x, int y) {
        resx = x;
        resy = y;
        renderer->SetResolution(resx, resy);
        minimized = (x * y == 0);
        if (!minimized) player->UpdateCameraResolution(resx, resy);
    }

    static void onLeftMouseButtonClick(int mods) {
        if (endScreenShown) return;
        player->ShowRay();
        sg::Object3D* obj = player->GetObject();
        enemyManager->AttackEnemies(obj->GetGlobalPosition(), obj->GlobalForward(), 300);

        SoundEngine->setSoundVolume(1);
        SoundEngine->play2D("res/sfx/laser.wav");
    }

    static void onWPressed(int mods) {
        if (endScreenShown) return;
        player->SetAccel(true);
        if (engineSound == NULL) {
            engineSound = SoundEngine->play2D("res/sfx/engine.wav", true, true);
        }
        SoundEngine->setSoundVolume(1);
        engineSound->setIsPaused(false);
    }

    static void onWReleased(int mods) {
        engineSound->setIsPaused(true);
        player->SetAccel(false);
    }

    static void onRPressed(int mods) {
        restart = true;
    }

    static void onMouseDrag(double xpos, double ypos) {
        player->ChangeDirection(
            -XROTSPEED * float(xpos - prevx) / float(resx),
            -YROTSPEED * float(ypos - prevy) / float(resy));

        glfwSetCursorPos(renderer->GetWindow(), resx / 2, resy / 2);
        prevx = resx / 2;
        prevy = resy / 2;
    }
#pragma endregion
};

int main(int argc, char* argv[]) {
    sgGame game;

    try {
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}