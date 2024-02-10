#include "Frustum.hpp"

Frustum::Frustum(const FPSCamera& cam) {
	this->update(cam);
}

void Frustum::update(const FPSCamera& cam) {
	auto& matrix = cam.getPVMatrix();

	auto rowX = glm::row(matrix, 0);
	auto rowY = glm::row(matrix, 1);
	auto rowZ = glm::row(matrix, 2);
	auto rowW = glm::row(matrix, 3);

	planes[FrustumFaces::Left] = glm::normalize(rowW + rowX);
	planes[FrustumFaces::Right] = glm::normalize(rowW - rowX);
	planes[FrustumFaces::Bottom] = glm::normalize(rowW + rowY);
	planes[FrustumFaces::Top] = glm::normalize(rowW - rowY);
	planes[FrustumFaces::Near] = glm::normalize(rowW + rowZ);
	planes[FrustumFaces::Far] = glm::normalize(rowW - rowZ);
}

bool Frustum::pointIn(const glm::vec3& point) {
	for (int i = 0; i < FrustumFaces::Count; i++)
		if (planes[i].signDistance(point) < 0)
			return false;

	return true;
}

bool Frustum::boxIn(const std::array<glm::vec3, 8>& vertices) {
	for (int i = 0; i < FrustumFaces::Count; i++) {
		int out = 0;
		int in = 0;

		for (int j = 0; j < 8; j++) {
			if (planes[i].signDistance(vertices[j]) < 0)
				out++;
			else
				in++;
		}

		if (!in)
			return false;
		else if (out) // maybe return something else for intersection
			return true;
	}

	return true;
}
