#include "Camera.hpp"

FPSCamera::FPSCamera(float f,
		float ar,
		glm::vec3 _position,
		glm::vec3 _up,
		float y,
		float p)
	:position{ camera_defaults::position },
	front{ camera_defaults::front },
	up{ camera_defaults::up },
	worldUp{ camera_defaults::up },
	right{ },
	yawAngle{ y },
	pitchAngle{ p },
	movementSpeed{ camera_defaults::speed },
	mouseSensitivity{ camera_defaults::sensitivity },
	zoom{ camera_defaults::zoom },
	view{},
	fov{f},
	aspectRatio{ar},
	projection{ glm::perspective(glm::radians(f), ar, 0.10f, 100.0f) } {

	this->update();

}

void FPSCamera::moveCamera(Directions d, float deltaTime) {
	const auto velocity = this->movementSpeed * deltaTime;
	switch (d) {
	case Forward:
		this->position += this->front * velocity;
		break;
	case Backwards:
		this->position -= this->front * velocity;
		break;
	case Right:
		this->position += this->right * velocity;
		break;
	case Left:
		this->position -= this->right * velocity;
	}

	if (!this->freeCamera)
		this->position.y = 0.0f;

	this->update();
}

void FPSCamera::handleMouseMovement(const glm::vec2& movement, bool constraintPitch) {
	const auto mov = movement * this->mouseSensitivity;
	this->yawAngle += mov.x;
	this->pitchAngle += mov.y;

	if (constraintPitch) {
		if (this->pitchAngle > 89.0f)
			this->pitchAngle = 89.0f;
		else if (this->pitchAngle < -89.0f)
			this->pitchAngle = -89.0f;
	}

	this->update();
}

void FPSCamera::handleMouseScroll(float yOffset) {
	this->zoom -= (float)yOffset;
	if (this->zoom < camera_defaults::minZoom)
		zoom = camera_defaults::minZoom;
	if (this->zoom > camera_defaults::maxZoom)
		zoom = camera_defaults::maxZoom;
}

void FPSCamera::resetCamera() {
	this->position = camera_defaults::position;
	this->front = camera_defaults::front;
	this->up = camera_defaults::up;
	this->yawAngle = camera_defaults::yaw;
	this->pitchAngle = camera_defaults::pitch;
	this->zoom = camera_defaults::zoom;
}

void FPSCamera::update() {
	this->front = glm::normalize(
		glm::vec3(
			cos(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle)),
			sin(glm::radians(this->pitchAngle)),
			sin(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle)))
	);

	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->front, this->right));

	// update view matrix
	this->view = glm::lookAt(
		this->position, this->position + this->front, this->up
	);
}