#pragma once

#include <iostream>
#include <vector>
#include <glm/vec3.hpp>

std::ostream& operator<<(std::ostream& os, const std::vector<float>& vec);

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec3);