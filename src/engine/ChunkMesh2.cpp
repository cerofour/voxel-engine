#include <iostream>
#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LightSource.hpp"
#include "ChunkMesh2.hpp"

static void checkGLError(const char* functionName) {
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error in " << functionName << ": ";
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			std::cerr << "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << "GL_OUT_OF_MEMORY";
			break;
		default:
			std::cerr << "Unknown error code: " << error;
			break;
		}
		std::cerr << std::endl;
	}
}

ChunkMesh2::ChunkMesh2(const glm::vec3& startPos)
	:vao{},
	shaderProgram{},
	textureLoader{},
	voxels{},
	id{} {

	this->startPosition = startPos;
	this->startPosition.y = 0;

	dlb::ShaderProgramBuilder shaderProgramBuilder{};
	this->shaderProgram = std::move(
		shaderProgramBuilder
		.shaderFromFile(SHADERSDIR "/chunk.vert")
		.fragmentFromFile(SHADERSDIR "/chunk.frag")
		.build());

	this->textureLoader
		.loadTexture(TEXTUREDIR "container.jpg");
}

void ChunkMesh2::generateChunk() {
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			for (int z = 0; z < CHUNKSIZE; z++) {
				this->voxels[x][y][z] = x + y + z + 1;
				//this->voxels[x][y][z] = (int)(0.1 * glm::simplex((glm::vec3(x, y, z) + this->startPosition)) + 1.0f) ? (x + y + z) : 0;
			}
		}
	}
}

#define IN_RANGE(v, l, h) (v >= l && v < h)

static bool isVoid(int x, int y, int z, ChunkMesh2::Voxel3DArray& voxels) {
	if (IN_RANGE(x, 0, CHUNKSIZE) && IN_RANGE(y, 0, CHUNKSIZE) && IN_RANGE(z, 0, CHUNKSIZE)) // verify if the coordinates are inside of the chunk
		if (voxels[x][y][z])
			return false;
	return true;
}

template<typename T>
static void pushVertex(std::vector<T>& cont, T x, T y, T z) {
	cont.push_back(x);
	cont.push_back(y);
	cont.push_back(z);
}

template<typename T>
static void pushVertex(std::vector<T>& cont, T x, T y) {
	cont.push_back(x);
	cont.push_back(y);
}

template<typename T>
static void pushVertex(std::vector<T>& cont, T x) {
	cont.push_back(x);
}

static void pushNormals(std::vector<float>& ns, float x, float y, float z) {
	pushVertex(ns, x, y, z);
	pushVertex(ns, x, y, z);
	pushVertex(ns, x, y, z);
	pushVertex(ns, x, y, z);
	pushVertex(ns, x, y, z);
	pushVertex(ns, x, y, z);
}

static void pushVoxelId(std::vector<float>& vs, float x) {
	vs.push_back(x);
	vs.push_back(x);
	vs.push_back(x);
	vs.push_back(x);
	vs.push_back(x);
	vs.push_back(x);
}

void ChunkMesh2::generateMesh() {
	std::vector<float> meshPositionData;
	std::vector<float> normals;
	std::vector<float> textureCoords;
	std::vector<float> voxelIds;

	for (int a = 0; a < CHUNKSIZE; a++) {
		for (int b = 0; b < CHUNKSIZE; b++) {
			for (int c = 0; c < CHUNKSIZE; c++) {
				auto voxel = (float)voxels[a][b][c];

				// WORLD COORDINATES
				auto x = static_cast<float>(a) + (this->startPosition.x * (float)CHUNKSIZE);
				auto y = static_cast<float>(b) + (this->startPosition.y * (float)CHUNKSIZE);
				auto z = static_cast<float>(c) + (this->startPosition.z * (float)CHUNKSIZE);

				if (voxel == 0)
					continue;

				// top face
				if (isVoid(a, b + 1, c, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x + 1, y + 1, z);
					pushVertex(meshPositionData, x + 1, y + 1, z + 1);
					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x, y + 1, z + 1);
					pushVertex(meshPositionData, x + 1, y + 1, z + 1);

					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, 0, 1, 0);
				}

				// bottom face
				if (isVoid(a, b - 1, c, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x, y, z);
					pushVertex(meshPositionData, x + 1, y, z);
					pushVertex(meshPositionData, x + 1, y, z + 1);
					pushVertex(meshPositionData, x, y, z);
					pushVertex(meshPositionData, x, y, z + 1);
					pushVertex(meshPositionData, x + 1, y, z + 1);

					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, 0, -1, 0);
				}

				// left face
				if (isVoid(a - 1, b, c, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x, y, z);
					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x, y, z + 1);
					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x, y + 1, z + 1);
					pushVertex(meshPositionData, x, y, z + 1);
					
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, -1, 0, 0);
				}

				// right face
				if (isVoid(a + 1, b, c, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x + 1, y, z);
					pushVertex(meshPositionData, x + 1, y + 1, z);
					pushVertex(meshPositionData, x + 1, y, z + 1);
					pushVertex(meshPositionData, x + 1, y + 1, z);
					pushVertex(meshPositionData, x + 1, y + 1, z + 1);
					pushVertex(meshPositionData, x + 1, y, z + 1);

					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, 1, 0, 0);
				}

				// front face
				if (isVoid(a, b, c - 1, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x, y, z);
					pushVertex(meshPositionData, x + 1, y, z);
					pushVertex(meshPositionData, x, y + 1, z);
					pushVertex(meshPositionData, x + 1, y + 1, z);
					pushVertex(meshPositionData, x + 1, y, z);

					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, 0, 0, 1);
				}

				// back face
				if (isVoid(a, b, c + 1, this->voxels)) {
					pushVoxelId(voxelIds, voxel);

					pushVertex(meshPositionData, x, y, z + 1);
					pushVertex(meshPositionData, x, y + 1, z + 1);
					pushVertex(meshPositionData, x + 1, y, z + 1);
					pushVertex(meshPositionData, x + 1, y, z + 1);
					pushVertex(meshPositionData, x + 1, y + 1, z + 1);
					pushVertex(meshPositionData, x, y + 1, z + 1);

					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 0.0f, 1.0f);
					pushVertex(textureCoords, 1.0f, 1.0f);

					pushNormals(normals, 0, 0, -1);
				}
			}
		}
	}

	this->vao.use()
		.addVertexAttribute(meshPositionData, 3, 0, GL_FALSE)
		.addVertexAttribute(normals, 3, 1, GL_FALSE)
		//.addVertexAttribute(textureCoords, 2, 2, GL_FALSE)
		.addVertexAttribute(voxelIds, 1, 2, GL_FALSE);

	this->vao.verticesN = meshPositionData.size() / 3;
}

void ChunkMesh2::render(const rendering::RenderingContext& ctx) {
	this->shaderProgram.use();
	this->textureLoader.enableTextures();

	glm::mat4 model{ 1.0f };

	//model = glm::translate(model, this->startPosition);
	   
	auto MVP = ctx.camera.getProjectionMatrix() * ctx.camera.getViewMatrix() * model;

	this->shaderProgram.setUniform("MVP", MVP);
	this->shaderProgram.setUniform("texture0", 0);
	this->shaderProgram.setUniform("modelMatrix", model);
	this->shaderProgram.setUniform("lightPosition", ctx.lightSource->getPosition());
	this->shaderProgram.setUniform("lightColor", ctx.lightSource->getColor());
	this->shaderProgram.setUniform("viewPosition", ctx.camera.getPosition());
	checkGLError(__FUNCTION__);

	this->vao.draw();
}
