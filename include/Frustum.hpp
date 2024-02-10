#pragma once

#include <array>
#include <cmath>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "Camera.hpp"

struct Plane {
	glm::vec4 plane;
	Plane() = default;

	Plane& operator=(const glm::vec4& rhs) {
		plane = rhs;
		return *this;
	}

	float signDistance(const glm::vec3& point) const {
		return glm::dot(glm::vec3(plane.x, plane.y, plane.z), point) + plane.w;
	}

	float distance(const glm::vec3& point) const {
		return abs(signDistance(point));
	}
};

class Frustum {
public:
	Frustum(const FPSCamera& cam);
	~Frustum() = default;

	enum FrustumFaces {
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count
	};

	void update(const FPSCamera& cam);

	bool pointIn(const glm::vec3& point);
	bool boxIn(const std::array<glm::vec3, 8>& box);

private:
	std::array<Plane, FrustumFaces::Count> planes;
};