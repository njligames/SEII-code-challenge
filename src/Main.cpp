#include <iostream>
#include <iomanip>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std::chrono_literals;

/**
 * A structure containing a loaded ECG waveform.
 */
struct ECG {
	int16_t **samples;
	int leads, channels, resolution;
	std::chrono::seconds length;
};

/**
 * Owns the OpenGL context, window, and application UI/UX behaviour.
 */
class ECGViewer {
public:
	ECGViewer() {
		glfwSetErrorCallback([](int error, const char *description) {
			std::cerr << "GLFW Error: " << description << "\n";
		});

		if (!glfwInit())
			throw std::runtime_error("Unable to Initialize GLFW");

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(windowWidth, windowHeight, "ISHNE Viewer", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Unable to Create Window");

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
			throw std::runtime_error("GLAD Failed to initialize OpenGL context");

		glfwShowWindow(window);
		glfwSetWindowUserPointer(window, this);

		glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
			auto viewer = static_cast<ECGViewer *>(glfwGetWindowUserPointer(window));
			viewer->closed = true;
		});
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int32_t width, int32_t height) {
			auto viewer = static_cast<ECGViewer *>(glfwGetWindowUserPointer(window));
			viewer->windowWidth = width;
			viewer->windowHeight = height;
		});
		glfwSetDropCallback(window, [](GLFWwindow *window, int32_t count, const char **paths) {
			auto viewer = static_cast<ECGViewer *>(glfwGetWindowUserPointer(window));
			viewer->LoadFile(paths[0]);
		});
		glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
			auto viewer = static_cast<ECGViewer *>(glfwGetWindowUserPointer(window));
			viewer->MoveView(viewer->moveSpeed * (int64_t)yoffset);
		});
		glfwSetKeyCallback(window, [](GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
			auto viewer = static_cast<ECGViewer *>(glfwGetWindowUserPointer(window));
			if (action != GLFW_RELEASE && key == GLFW_KEY_LEFT) {
				viewer->MoveView(-viewer->moveSpeed);
			} else if (action != GLFW_RELEASE && key == GLFW_KEY_RIGHT) {
				viewer->MoveView(+viewer->moveSpeed);
			}
		});
	}

	~ECGViewer() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void RenderLoop() {
		while (!closed) {
			glClear(GL_COLOR_BUFFER_BIT);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void LoadFile(std::filesystem::path filename) {
		std::cout << "Loading ISHNE: " << filename << "\n";
	}
	
	void MoveView(std::chrono::seconds s) {
		std::cout << "Move view: " << s.count() << "\n";
	}
	
private:
	GLFWwindow *window;
	int windowWidth = 1080, windowHeight = 720;
	std::chrono::seconds moveSpeed = 10s;
	bool closed = false;
	ECG *ecg = nullptr;
};

int main(int argc, char **argv) {
	ECGViewer viewer;
	viewer.RenderLoop();
	return 0;
}
