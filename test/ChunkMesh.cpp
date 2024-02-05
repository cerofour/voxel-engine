#include <iostream>
#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ChunkMesh.hpp"

#define PRINT_VALUES(...) \
	do { \
		std::cout << "Values: "; \
		printValues(__VA_ARGS__); \
	} while (false)

// Helper function for printing variadic arguments
template<typename T>
void printValues(const T& value) {
	std::cout << value << std::endl;
}

template<typename T, typename... Args>
void printValues(const T& first, const Args&... args) {
	std::cout << first << ", ";
	printValues(args...);
}

ChunkMesh::ChunkMesh(const glm::vec3& startPos)
	:vao{},
	shaderProgram{},
	textureLoader{} {

	this->startPosition = startPos;
	this->startPosition.y = 0;

	dlb::ShaderProgramBuilder shaderProgramBuilder{};
	this->shaderProgram = std::move(
		shaderProgramBuilder
		.shaderFromFile(SHADERSDIR "/going3d.vert")
		.fragmentFromFile(SHADERSDIR "/colorfulWithLight.frag")
		.build());

	this->textureLoader
		.loadTexture(TEXTUREDIR "container.jpg");
}

void ChunkMesh::generateChunk() {
	static const float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,

	};
	static const float textureCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	};
	static const float lightNormals[] = {
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
	};

	for (int x = 0; x < ChunkMesh::chunkSize().x; x++)
		for (int y = 0; y < ChunkMesh::chunkSize().y; y++)
			for (int z = 0; z < ChunkMesh::chunkSize().z; z++)
				this->cubePositions.push_back(this->startPosition + glm::vec3(x, y, z ));

	this->vao.use()
		.addVertexAttribute(cubeVertices, sizeof(cubeVertices) / sizeof(float), 3, 0, GL_FALSE)
		.addVertexAttribute(textureCoords, sizeof(textureCoords) / sizeof(float), 2, 1, GL_FALSE)
		.addVertexAttribute(lightNormals, sizeof(lightNormals) / sizeof(float), 3, 2, GL_FALSE);

	this->vao.verticesN = 36;
}

void ChunkMesh::prepareRendering(const rendering::RenderingContext& ctx) {
}

void ChunkMesh::render(const rendering::RenderingContext& ctx) {

	this->shaderProgram.use();
	this->textureLoader.enableTextures();
	this->shaderProgram.setUniform("texture0", 0);

	for (auto i = 0; i < this->cubePositions.size(); i++) {
		glm::mat4 model{ 1.0f };

		// move the cube
		model = glm::translate(model, cubePositions[i]);

		auto MVP = ctx.camera.getProjectionMatrix() * ctx.camera.getViewMatrix() * model;

		this->shaderProgram.setUniform("modelMatrix", model);
		this->shaderProgram.setUniform("lightColor", ctx.lightSource->getColor());
		this->shaderProgram.setUniform("MVP", MVP);

		shaderProgram.setUniform("MVP", MVP);
		this->vao.draw();
	}
}