#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace sg {
	using InputCommand = unsigned int;
	constexpr auto Mouse_Left_Down = 0;
	constexpr auto Mouse_Left_Up = 1;
	constexpr auto Mouse_Right_Down = 2;
	constexpr auto Mouse_Right_Up = 3;
	constexpr auto Mouse_Position = 4;
	constexpr auto Key_Esc_Down = 100;
	constexpr auto Key_Esc_Up = 101;
	constexpr auto Key_Ctrl_Down = 102;
	constexpr auto Key_Ctrl_Up = 103;
	constexpr auto Key_Space_Down = 104;
	constexpr auto Key_Space_Up = 105;
	constexpr auto Key_W_Down = 110;
	constexpr auto Key_W_Up = 111;
	constexpr auto Key_A_Down = 112;
	constexpr auto Key_A_Up = 113;
	constexpr auto Key_S_Down = 114;
	constexpr auto Key_S_Up = 115;
	constexpr auto Key_D_Down = 116;
	constexpr auto Key_D_Up = 117;
	constexpr auto Window_Resize = 200;

	typedef void (*sgCursorPosFun)(double xpos, double ypos);
	typedef void (*sgKeyOrMouseFun)(int mods);
	typedef void (*sgWindowSizeFun)(int width, int height);

	struct KeyOrMouseFunction {
		GLFWwindow* window;
		int key;
		int action;

		bool operator==(KeyOrMouseFunction other) const {
			return (other.window == this->window && other.key == this->key && other.action == this->action);
		}
	};

	struct KeyOrMouseFunctionHasher {
		std::size_t operator()(const KeyOrMouseFunction& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			return ((hash<GLFWwindow*>()(k.window)
				^ (hash<int>()(k.key) << 1)) >> 1)
				^ (hash<int>()(k.action) << 1);
		}
	};

	class InputManager {
	private:
		static InputManager _instance;
		static bool _initialized;

		InputManager() {

		}

		static std::unordered_map<GLFWwindow*, bool> _mouseButtonCallbackSet;
		static std::unordered_map<GLFWwindow*, bool> _keyCallbackSet;
		static std::unordered_map<GLFWwindow*, bool> _mousePositionCallbackSet;
		static std::unordered_map<GLFWwindow*, bool> _windowResizeCallbackSet;

		static std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher> _mouseButtonFunctions;
		static std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher> _keyFunctions;
		static std::unordered_map<GLFWwindow*, sgCursorPosFun> _mousePositionFunctions;
		static std::unordered_map<GLFWwindow*, sgWindowSizeFun> _windowResizeFunctions;

		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);

		static void WindowResizeCallback(GLFWwindow* window, int width, int height);

		void BindingError() {
			printf("Error: The key was not valid\n");
		}

	public:
		static InputManager* Instance();

		void BindInput(GLFWwindow* window, InputCommand cmd, sgKeyOrMouseFun callback) {
			KeyOrMouseFunction f = KeyOrMouseFunction();
			f.window = window;
			bool isMouse = false;
			switch (cmd) {
			case Mouse_Left_Down:
				f.key = GLFW_MOUSE_BUTTON_LEFT; f.action = 1;
				isMouse = true;
				break;
			case Mouse_Left_Up:
				f.key = GLFW_MOUSE_BUTTON_LEFT; f.action = 0;
				isMouse = true;
				break;
			case Mouse_Right_Down:
				f.key = GLFW_MOUSE_BUTTON_RIGHT; f.action = 1;
				isMouse = true;
				break;
			case Mouse_Right_Up:
				f.key = GLFW_MOUSE_BUTTON_RIGHT; f.action = 0;
				isMouse = true;
				break;
			case Key_Esc_Down:
				f.key = GLFW_KEY_ESCAPE; f.action = 1;
				break;
			case Key_Esc_Up:
				f.key = GLFW_KEY_ESCAPE; f.action = 0;
				break;
			case Key_Ctrl_Down:
				f.key = GLFW_KEY_LEFT_CONTROL; f.action = 1;
				break;
			case Key_Ctrl_Up:
				f.key = GLFW_KEY_LEFT_CONTROL; f.action = 0;
				break;
			case Key_Space_Down:
				f.key = GLFW_KEY_SPACE; f.action = 1;
				break;
			case Key_Space_Up:
				f.key = GLFW_KEY_SPACE; f.action = 0;
				break;
			case Key_W_Down:
				f.key = GLFW_KEY_W; f.action = 1;
				break;
			case Key_W_Up:
				f.key = GLFW_KEY_W; f.action = 0;
				break;
			case Key_A_Down:
				f.key = GLFW_KEY_A; f.action = 1;
				break;
			case Key_A_Up:
				f.key = GLFW_KEY_A; f.action = 0;
				break;
			case Key_S_Down:
				f.key = GLFW_KEY_S; f.action = 1;
				break;
			case Key_S_Up:
				f.key = GLFW_KEY_S; f.action = 0;
				break;
			case Key_D_Down:
				f.key = GLFW_KEY_D; f.action = 1;
				break;
			case Key_D_Up:
				f.key = GLFW_KEY_D; f.action = 0;
				break;
			default:
				BindingError();
				return;
			}
			if (isMouse) {
				_mouseButtonFunctions[f] = callback;
				if (!_mouseButtonCallbackSet[window]) {
					glfwSetMouseButtonCallback(window, MouseButtonCallback);
					_mouseButtonCallbackSet[window] = true;
				}
			} else {
				_keyFunctions[f] = callback;
				if (!_keyCallbackSet[window]) {
					glfwSetKeyCallback(window, KeyCallback);
					_keyCallbackSet[window] = true;
				}
			}
		}

		void BindInput(GLFWwindow* window, InputCommand cmd, sgCursorPosFun callback) {
			switch (cmd) {
			case Mouse_Position:
				_mousePositionFunctions[window] = callback;
				if (!_mousePositionCallbackSet[window]) {
					glfwSetCursorPosCallback(window, MousePositionCallback);
					_mousePositionCallbackSet[window] = true;
				}
				break;
			default:
				BindingError();
				break;
			}
		}

		void BindInput(GLFWwindow* window, InputCommand cmd, sgWindowSizeFun callback) {
			switch (cmd) {
			case Window_Resize:
				_windowResizeFunctions[window] = callback;
				if (!_windowResizeCallbackSet[window]) {
					glfwSetWindowSizeCallback(window, WindowResizeCallback);
					_windowResizeCallbackSet[window] = true;
				}
				break;
			default:
				BindingError();
				break;
			}
		}
	};

	InputManager InputManager::_instance = InputManager::InputManager();
	bool InputManager::_initialized = false;
	std::unordered_map<GLFWwindow*, bool> InputManager::_mouseButtonCallbackSet = std::unordered_map<GLFWwindow*, bool>();
	std::unordered_map<GLFWwindow*, bool> InputManager::_mousePositionCallbackSet = std::unordered_map<GLFWwindow*, bool>();
	std::unordered_map<GLFWwindow*, bool> InputManager::_keyCallbackSet = std::unordered_map<GLFWwindow*, bool>();
	std::unordered_map<GLFWwindow*, bool> InputManager::_windowResizeCallbackSet = std::unordered_map<GLFWwindow*, bool>();

	std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher> InputManager::_mouseButtonFunctions = std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher>();
	std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher> InputManager::_keyFunctions = std::unordered_map<KeyOrMouseFunction, sgKeyOrMouseFun, KeyOrMouseFunctionHasher>();
	std::unordered_map<GLFWwindow*, sgCursorPosFun> InputManager::_mousePositionFunctions = std::unordered_map<GLFWwindow*, sgCursorPosFun>();
	std::unordered_map<GLFWwindow*, sgWindowSizeFun> InputManager::_windowResizeFunctions = std::unordered_map<GLFWwindow*, sgWindowSizeFun>();

	InputManager* InputManager::Instance() {
		if (!_initialized) {
			_instance = InputManager();
			_initialized = true;
		}
		return &_instance;
	}

	void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		KeyOrMouseFunction f = KeyOrMouseFunction();
		f.window = window; f.key = button; f.action = action;
		if (_mouseButtonFunctions[f]) {
			_mouseButtonFunctions[f](mods);
		}
	}

	void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		KeyOrMouseFunction f = KeyOrMouseFunction();
		f.window = window; f.key = key; f.action = action;
		if (_keyFunctions[f]) {
			_keyFunctions[f](mods);
		}
	}

	void InputManager::MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
		if (_mousePositionFunctions[window]) {
			_mousePositionFunctions[window](xpos, ypos);
		}
	}

	void InputManager::WindowResizeCallback(GLFWwindow* window, int width, int height) {
		if (_windowResizeFunctions[window]) {
			_windowResizeFunctions[window](width, height);
		}
	}
}