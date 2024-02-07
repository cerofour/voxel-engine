#pragma once

#include <array>

using uint = unsigned int;

template<typename T, uint x, uint y>
class Array2D {
public:
	Array2D() = default;
	~Array2D() = default;

public:
	T& at(uint x, uint y) {
		return data[x][y];
	}
private:
	std::array < std::array<T, y>, x > data;
};

template<typename T, uint x, uint y, uint z>
class Array3D {
public:
	Array3D() = default;
	~Array3D() = default;

public:
	T& at(uint x, uint y, uint z) {
		return data[x][y][z];
	}
private:
	std::array < std::array<std::array<T, z>, y>, x > data;
};