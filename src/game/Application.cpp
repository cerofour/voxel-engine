#include <iostream>

#include <glad/glad.h>

#include "Application.hpp"

static void frameBufferSizeCallback(GLFWwindow* _, int w, int h) {
	glViewport(0, 0, w, h);
	// update camera projection matrix
	Application::getInstance()->getCamera().updateAspectRatio((float)w / (float)h);
}

static void mouseCallback(GLFWwindow* win, double xPosIn, double yPosIn) {
	float xpos = static_cast<float>(xPosIn);
	float ypos = static_cast<float>(yPosIn);

	if (Application::getInstance()->getFirstMouseFocus()) {
		Application::getInstance()->getLastMouseX() = xpos;
		Application::getInstance()->getLastMouseY() = ypos;
		Application::getInstance()->getFirstMouseFocus() = false;
	}

	auto xoffset = Application::getInstance()->getLastMouseX() - xpos;
	auto yoffset = ypos - Application::getInstance()->getLastMouseY();

	Application::getInstance()->getLastMouseX() = xpos;
	Application::getInstance()->getLastMouseY() = ypos;

	Application::getInstance()->getCamera().handleMouseMovement(glm::vec2(xoffset, yoffset));
}

static void scrollCallback(GLFWwindow* _, double __, double yoffset) {
	Application::getInstance()->getCamera().handleMouseScroll(yoffset);
}

static void errorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static GLFWwindow* createWindow(int width, int height, const std::string& windowTitle) {
	auto window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);

	if (window == nullptr) {
		std::cerr << "failed to create glfw window" << std::endl;
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "failed to initialize glad" << std::endl;
		std::exit(-1);
	}
	glViewport(0, 0, width, height);

	return window;
}

static void enableOpenGLOptions() {
	glEnable(GL_DEPTH_TEST);
}

static void setCallbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scrollCallback);
}

Application::Application(float fov, int winX, int winY, const std::string& winTitle)
	:camera{ fov, (float)winX / (float)winY },
	renderer{} {
	this->fov = fov;
	this->width = winX;
	this->height = winY;
	this->windowTitle = winTitle;
}

Application* Application::instance = nullptr;

Application* Application::getInstance() {
	if (!instance) {
		instance = new Application{ 90.0f, 800, 600, "VoxelEngine" };
	}
	return instance;
}

void Application::onInit() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSetErrorCallback(errorCallback);

	this->window = createWindow(this->width, this->height, this->windowTitle);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "failed to initialize glad" << std::endl;
		std::exit(-1);
	}

	enableOpenGLOptions();
	setCallbacks(this->window);

	this->lastMouseX = (float)this->width / 2.0;
	this->lastMouseY = (float)this->height / 2.0;
}

void Application::onUpdate() {
	this->processInput();
}

void Application::onDelete() {

}

void Application::processInput() {
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);

	// display frame buffer size
	if (glfwGetKey(this->window, GLFW_KEY_I) == GLFW_PRESS) {
		int currentWidth, currentHeight;
		glfwGetFramebufferSize(this->window, &currentWidth, &currentHeight);
		std::cout << "Window size: " << currentWidth << "x" << currentHeight << std::endl;
	}

	/*
	* Movement managenment
	*/
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
		this->camera.moveCamera(FPSCamera::Directions::Forward, this->deltaTime);

	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
		this->camera.moveCamera(FPSCamera::Directions::Backwards, this->deltaTime);

	// To move sideways first we calculate the cross product between the UP and the FRONT vector, that
	// gives us the vector to move along the sides.
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->camera.moveCamera(FPSCamera::Directions::Right, this->deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->camera.moveCamera(FPSCamera::Directions::Left, this->deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		this->camera.resetCamera();

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		this->moveLight = !this->moveLight;
}
