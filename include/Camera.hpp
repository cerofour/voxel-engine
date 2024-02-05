#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace camera_defaults {
	const glm::vec3 position{ 0.0f, 0.0f, -3.0f };
	const glm::vec3 front{ 0.0f, 0.0f, -1.0f };
	const glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	const float yaw{ 90.0f };
	const float pitch{ 0.0f };
	const float speed{ 10.0f };
	const float sensitivity{ 0.05f };
	const float zoom{ 45.0f };
	const float minZoom{ 1.0f };
	const float maxZoom{ 60.0f };
}

class FPSCamera {
public:
	enum Directions {
		Forward,
		Backwards,
		Left,
		Right,
	};

	FPSCamera(float fov,
		float aspectRatio,
		glm::vec3 _position = camera_defaults::position,
		glm::vec3 _up = camera_defaults::up,
		float y = camera_defaults::yaw,
		float p = camera_defaults::pitch);

public:
	void moveCamera(Directions d, float deltaTime);
	void handleMouseMovement(const glm::vec2& movement, bool constraintPitch = true);
	void handleMouseScroll(float yOffset);
	void resetCamera();

public:
	float getFOV() const {
		return this->fov;
	}

	float getAspectRatio() const {
		return this->aspectRatio;
	}

	const auto& getMovementSpeed() {
		return this->movementSpeed;
	}

	const auto& getViewMatrix() {
		return this->view;
	}

	const auto& getProjectionMatrix() {
		return this->projection;
	}

	const auto& getPosition() {
		return this->position;
	}

	const auto& getPVMatrix() {
		return this->projection * this->view;
	}

	auto setMovementSpeed(float s) {
		this->movementSpeed = s;
	}

	auto toggleFreeCamera() {
		this->freeCamera = !this->freeCamera;
	}

	auto incrementMovementSpeed(float offset) {
		this->movementSpeed += offset;
	}

private:
	void update();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 worldUp;
	glm::vec3 right;

	float yawAngle;
	float pitchAngle;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	float fov;
	float aspectRatio;

	bool freeCamera = false;

	glm::mat4 view;
	glm::mat4 projection;
};