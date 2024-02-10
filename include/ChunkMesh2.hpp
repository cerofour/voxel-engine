#pragma once

#include <vector>
#include <array>
#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Rendering.hpp"
#include "ShaderCreation.hpp"
#include "TextureLoader.h"
#include "Config.hpp"

// @struct ChunkMesh
// @brief collection of all the vertices conforming a chunk and responsible of
//			calculating which of those vertices are rendered to the screen.
//
class ChunkMesh2 : public rendering::RenderObject {
public:
	using u8 = uint8_t;
	using Voxel3DArray = std::array<std::array<std::array<uint8_t, CHUNKSIZE>, CHUNKSIZE>, CHUNKSIZE>;

	ChunkMesh2(const glm::vec3& startPosition);
	~ChunkMesh2() = default;

public:
	// @brief generates a chunk. Populating the @vertices vector.
	// @param startPosition the starting X and Z positions in which a chunk will
	//			be generated. The starting Y component of a chunk will always be Y = 0
	//			the chunk will be generated in the directions of +X +Z and +Y
	void generateChunk();

	// @brief Naive oclusion culling algorithm
	void generateMesh(const rendering::RenderingContext& ctx);

	void render(const rendering::RenderingContext& ctx);

	bool inFrustum(const rendering::RenderingContext& ctx);

public:
	rendering::MultipleBufferVAO& getVAO() {
		return this->vao;
	}

	dlb::ShaderProgram& getSP() {
		return this->shaderProgram;
	}

	Voxel3DArray& getVoxels() {
		return this->voxels;
	}

	void setId(int v) {
		this->id = v;
	}

	const glm::vec3& getStartPosition() const {
		return startPosition;
	}

private:

	// @vertices is a collection of all the vertices that conform a chunk,
	// even those that aren't visible to the camera (and thus will not be rendered)
	// @visibleVertices contains only the vertices that are visible by the camera.

	//std::vector<float> coordVertices;
	//std::vector<float> textureVertices;
	//std::vector<float> visibleVertices;

	Voxel3DArray voxels;

	int id;

	glm::vec3 startPosition;

	dlb::ShaderProgram shaderProgram;
	dlb::TextureLoader textureLoader;

	rendering::MultipleBufferVAO vao;

	bool meshUpdate = true;
};