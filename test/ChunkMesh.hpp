#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Rendering.hpp"
#include "ShaderCreation.hpp"
#include "TextureLoader.h"

// @struct ChunkMesh
// @brief collection of all the vertices conforming a chunk and responsible of
//			calculating which of those vertices are rendered to the screen.
//
class ChunkMesh : public rendering::RenderObject {
public:
	static const glm::ivec3 chunkSize() {
		return { 16, 16, 16 };
	}

public:
	ChunkMesh(const glm::vec3& startPosition);
	~ChunkMesh() = default;

public:
	// @brief generates a chunk. Populating the @vertices vector.
	// @param startPosition the starting X and Z positions in which a chunk will
	//			be generated. The starting Y component of a chunk will always be Y = 0
	//			the chunk will be generated in the directions of +X +Z and +Y
	void generateChunk();

	// @brief Naive oclusion culling algorithm
	void prepareRendering(const rendering::RenderingContext& ctx);

	void render(const rendering::RenderingContext& ctx);

public:
	rendering::MultipleBufferVAO& getVAO() {
		return this->vao;
	}

	dlb::ShaderProgram& getSP() {
		return this->shaderProgram;
	}

private:
	// @vertices is a collection of all the vertices that conform a chunk,
	// even those that aren't visible to the camera (and thus will not be rendered)
	// @visibleVertices contains only the vertices that are visible by the camera.
	
	//std::vector<float> coordVertices;
	//std::vector<float> textureVertices;
	//std::vector<float> visibleVertices;

	std::vector<glm::vec3> cubePositions;

	glm::vec3 startPosition;

	dlb::ShaderProgram shaderProgram;
	dlb::TextureLoader textureLoader;

	rendering::MultipleBufferVAO vao;
};