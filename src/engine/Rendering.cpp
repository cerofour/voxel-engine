#include "Rendering.hpp"

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

using namespace rendering;

// @brief Copies the data of a vertex attribute into the VBO,
// sets the layout of the vertex attribute with glVertexAttribPointer and
// finally enables the attribute.
// @warning ALWAYS call use() before calling this function, otherwise
//		the vertex attributes will not be associated with the VAO.
// @param attribData
// @param attribSize amount of elements an attribute has per vertex,
//		 e.g. a texture attribute may have 2 attributes only.
// @param attribId self-explanatory, the id of the attribute that is used in
//		the vertex shaders.
// @param normalize if true openGl will normalize the values in attribData
// @returns *this so it can be used consecutively to add any amount of vertex
//		attributes
MultipleBufferVAO& MultipleBufferVAO::addVertexAttribute(
	const std::vector<float>& attribData,
	GLuint attribSize,
	GLuint attribId,
	GLboolean normalize) {

	GLuint vbo{};

	glGenBuffers(1, &vbo);
	this->vbos.push_back(vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, attribData.size() * sizeof(float), attribData.data(), GL_STATIC_DRAW);

	checkGLError(__FUNCTION__);

	glVertexAttribPointer(attribId,
		attribSize,
		GL_FLOAT,
		normalize,
		attribSize * sizeof(float),
		(GLvoid*)0
	);

	glEnableVertexAttribArray(attribId);

	return *this;
}

MultipleBufferVAO& MultipleBufferVAO::addVertexAttribute(
	const std::vector<int>& attribData,
	GLuint attribSize,
	GLuint attribId,
	GLboolean normalize) {

	GLuint vbo{};

	glGenBuffers(1, &vbo);
	this->vbos.push_back(vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, attribData.size() * sizeof(int), attribData.data(), GL_STATIC_DRAW);

	checkGLError(__FUNCTION__);

	glVertexAttribPointer(attribId,
		attribSize,
		GL_INT,
		normalize,
		attribSize * sizeof(int),
		(GLvoid*)0
	);

	glEnableVertexAttribArray(attribId);

	return *this;
}


MultipleBufferVAO& MultipleBufferVAO::addVertexAttribute(
	const float* attribData,
	size_t attribDataCount,
	GLuint attribSize,
	GLuint attribId,
	GLboolean normalize) {

	GLuint vbo{};

	glGenBuffers(1, &vbo);
	this->vbos.push_back(vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, attribDataCount * sizeof(float), attribData, GL_STATIC_DRAW);

	checkGLError(__FUNCTION__);

	glVertexAttribPointer(attribId,
		attribSize,
		GL_FLOAT,
		normalize,
		attribSize * sizeof(float),
		(GLvoid*)0
	);

	glEnableVertexAttribArray(attribId);

	return *this;
}

Renderer& Renderer::attatchObject(RenderObject* obj) {
	this->objects.push_back(obj);
	return *this;
}

void Renderer::render(const RenderingContext& ctx) {

	int totalObjects = this->objects.size();

	int renderedObjects = 0;

	for (auto& object : this->objects) {
		if (object->inFrustum(ctx)) {
			renderedObjects++;
			object->render(ctx);
		}
	}

	std::cout << "Total objects: " << totalObjects << " Only rendered: " << renderedObjects << '\n';
}