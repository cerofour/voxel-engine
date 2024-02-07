#include "Printing.hpp"

// Overload the << operator for std::ostream and std::vector<float>
std::ostream& operator<<(std::ostream& os, const std::vector<float>& vec) {
	os << "[ ";

	int i = 0;
	int lineno = 0;

	for (const auto& element : vec) {

		if (i++ % 3 == 0) {
			lineno++;
			os << std::endl << lineno << " ";
		}

		os << element << " ";
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "X: " << v.x << " Y: " << v.y << " Z: " << v.z;
	return os;
}
