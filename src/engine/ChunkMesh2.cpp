#include <iostream>
#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LightSource.hpp"
#include "ChunkMesh2.hpp"
#include "Printing.hpp"
#include "Frustum.hpp"

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

	dlb::ShaderProgramBuilder shaderProgramBuilder{};
	this->shaderProgram = std::move(
		shaderProgramBuilder
		.shaderFromFile(SHADERSDIR "/chunk.vert")
		.fragmentFromFile(SHADERSDIR "/chunk.frag")
		.build());

	this->textureLoader
		.loadTexture(TEXTUREDIR "container.jpg");
}

static constexpr const int caveY = 36;

static int generateTerrain(const glm::vec3& worldPos) {

	if (worldPos.y <= caveY) {
		auto noiseVal = glm::perlin(glm::vec3(worldPos.x, worldPos.y, worldPos.z) * 0.05f);
		return noiseVal < 0 ? 0 : worldPos.y + worldPos.z;
	}
	
	auto surfaceY = 40 + glm::perlin(glm::vec2(worldPos.x, worldPos.z) * 0.05f) * 8;
	return (worldPos.y < surfaceY) ? worldPos.y + worldPos.z : 0;
}

// PUT HERE THE TERRAIN GENERATION ALGORITHM
void ChunkMesh2::generateChunk() {

	auto cx = this->startPosition.x * CHUNKSIZE;
	auto cy = this->startPosition.y * CHUNKSIZE;
	auto cz = this->startPosition.z * CHUNKSIZE;

	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int z = 0; z < CHUNKSIZE; z++) {
			for (int y = 0; y < CHUNKSIZE; y++) {
				this->voxels[x][y][z] = generateTerrain(glm::vec3(cx + x, cy + y, cz + z));
			}
		}
	}
}

#define IN_RANGE(v, l, h) (v >= l && v < h)

static bool isVoid(const glm::vec3& chunkRelPos, const glm::vec3& worldPos, const rendering::RenderingContext& ctx) {

	//std::cout << chunkRelPos << " " << worldPos << '\n';

	auto wx = (worldPos.x / (float)CHUNKSIZE);
	auto wy = (worldPos.y / (float)CHUNKSIZE);
	auto wz = (worldPos.z / (float)CHUNKSIZE);

	auto cx = (CHUNKSIZE * (chunkRelPos.x == -1)) + (int)chunkRelPos.x % CHUNKSIZE;
	auto cy = (CHUNKSIZE * (chunkRelPos.y == -1)) + (int)chunkRelPos.y % CHUNKSIZE; // always transform Y to negative because of some shit with the camera.
	auto cz = (CHUNKSIZE * (chunkRelPos.z == -1)) + (int)chunkRelPos.z % CHUNKSIZE;

	// coordinates are outside of the world, thus chunk doesn't exists (void)
	if (wy < 0 || wy >= WORLDSIZE || wx < 0 || wx >= WORLDSIZE || wz < 0 || wz >= WORLDSIZE)
		return true;

	auto chunk = ctx.world->at(wx, wy, wz);
	//if (IN_RANGE(cx, 0, CHUNKSIZE) && IN_RANGE(cy, 0, CHUNKSIZE) && IN_RANGE(cz, 0, CHUNKSIZE))
	if (chunk->getVoxels()[cx][cy][cz])
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

void ChunkMesh2::generateMesh(const rendering::RenderingContext& ctx) {

	if (!this->meshUpdate)
		return;

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
				if (isVoid(glm::vec3(a, b + 1, c), glm::vec3(x, y + 1, z), ctx)) {
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
				if (isVoid(glm::vec3(a, b - 1, c), glm::vec3(x, y - 1, z), ctx)) {
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
				if (isVoid(glm::vec3(a - 1, b, c), glm::vec3(x - 1, y, z), ctx)) {
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
				if (isVoid(glm::vec3(a + 1, b, c), glm::vec3(x + 1, y, z), ctx)) {
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
				if (isVoid(glm::vec3(a, b, c - 1), glm::vec3(x, y, z - 1), ctx)) {
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
				if (isVoid(glm::vec3(a, b, c + 1), glm::vec3(x, y, z + 1), ctx)) {
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
	this->meshUpdate = false;
}

void ChunkMesh2::render(const rendering::RenderingContext& ctx) {

	this->generateMesh(ctx);

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

bool ChunkMesh2::inFrustum(const rendering::RenderingContext& ctx) {
	auto& pos = this->startPosition;

	// calculate all 8 vertices of the chunk

	auto scale = (float)CHUNKSIZE;

	std::array<glm::vec3, 8> vertices = {
		pos * scale, // (pos + glm::vec3(0.0f, 0.0f, 0.0f)
		(pos + glm::vec3(1.0f, 0.0f, 0.0f)) * scale,
		(pos + glm::vec3(0.0f, 1.0f, 0.0f)) * scale,
		(pos + glm::vec3(0.0f, 0.0f, 1.0f)) * scale,
		(pos + glm::vec3(1.0f, 1.0f, 0.0f)) * scale,
		(pos + glm::vec3(1.0f, 0.0f, 1.0f)) * scale,
		(pos + glm::vec3(0.0f, 1.0f, 1.0f)) * scale,
		(pos + glm::vec3(1.0f, 1.0f, 1.0f)) * scale,
	};
	
	return ctx.frustum->boxIn(vertices);
}
