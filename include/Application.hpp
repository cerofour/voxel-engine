#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "Rendering.hpp"
#include "Camera.hpp"

class Application {
private:
	static Application* instance;

	Application(float fov, int winX, int winY, const std::string& winTitle);

	~Application() = default;

public:
	static Application* getInstance();

public:
	void onInit();

	void onUpdate();

	void onDelete();

private:
	void processInput();

public:
	bool& doMoveLight() {
		return this->moveLight;
	}

	auto getWindow() {
		return this->window;
	}

	bool& getFirstMouseFocus() {
		return this->firstMouseFocus;
	}

	float& getLastMouseX() {
		return this->lastMouseX;
	}

	float& getLastMouseY() {
		return this->lastMouseY;
	}

	float& getDeltaTime() {
		return this->deltaTime;
	}

	float& getLastFrame() {
		return this->lastFrame;
	}

	FPSCamera& getCamera() {
		return this->camera;
	}

	rendering::RenderingContext getContext() {
		rendering::RenderingContext ctx = {
			this->camera,
			this->deltaTime,
			this->lastFrame,
			nullptr,
		};
		return ctx;
	}

	float getAspectRatio() {
		return (float)this->width / (float)this->height;
	}

private:
	GLFWwindow* window = nullptr;
	int width;
	int height;
	std::string windowTitle;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool firstMouseFocus = true;
	bool fixedPlane = true;
	bool polygonMode = false;

	float lastMouseX = 0.0;
	float lastMouseY = 0.0;
	float yawAngle = -90.0;
	float pitchAngle = 0.0f;
	float fov = 45.0f;

	bool moveLight = false;

	FPSCamera camera;

	rendering::Renderer renderer;
};