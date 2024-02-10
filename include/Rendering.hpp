#pragma once

#include <vector>
#include <functional>

#include <glad/glad.h>

#include "Config.hpp"
#include "Types.hpp"
#include "ShaderCreation.hpp"
#include "Camera.hpp"

class Frustum;
class LightSource;
class ChunkMesh2;

namespace rendering {
	struct RenderingContext {
		FPSCamera& camera;
		float deltaTime;
		float lastTime;
		Frustum* frustum;
		LightSource* lightSource;
		Array3D<ChunkMesh2*, WORLDSIZE, WORLDSIZE, WORLDSIZE>* world;
	};

	// @class RenderObject
	// @brief Virtual class to represent any object that can be rendered to the screen.
	class RenderObject {
	public:
		virtual void render(const RenderingContext& ctx) = 0;
		virtual bool inFrustum(const RenderingContext& ctx) = 0;
	};

	/*
	* @struct VAO
	* @brief Abstracts the entire process of managing a VAO and a VBO, definning vertex
	* attributes and writting data to the VBO.
	*/
	struct MultipleBufferVAO {
		GLuint vao;
		std::vector<GLuint> vbos;

		GLsizeiptr verticesN;

		GLsizeiptr vboDataOffset;

		bool firstWrite;

		MultipleBufferVAO()
			:vboDataOffset{ 0 },
			verticesN{ 0 },
			vbos{},
			firstWrite{ true } {
			glGenVertexArrays(1, &this->vao);
		}

		~MultipleBufferVAO() {
			glDeleteVertexArrays(1, &this->vao);
			for(const auto& vbo : this->vbos)
				glDeleteBuffers(1, &vbo);
		}

		MultipleBufferVAO& use() {
			glBindVertexArray(this->vao);
			return *this;
		}

		void clear() {
			glDeleteBuffers(vbos.size(), this->vbos.data());
		}

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
		MultipleBufferVAO& addVertexAttribute(const std::vector<float>& attribData,
				GLuint attribSize,
				GLuint attribId,
				GLboolean normalize);

		MultipleBufferVAO& addVertexAttribute(const std::vector<int>& attribData,
			GLuint attribSize,
			GLuint attribId,
			GLboolean normalize);

		// @brief Adds a vertex attribute. Uses a pointer to an array of floats
		//		instead of a vector.
		// @param attribDataCount is the number of elements in the array
		MultipleBufferVAO& addVertexAttribute(const float* attribData,
			size_t attribDataCount,
			GLuint attribSize,
			GLuint attribId,
			GLboolean normalize);

		void draw() const {
			glBindVertexArray(this->vao);
			glDrawArrays(GL_TRIANGLES, 0, this->verticesN);
		}
	};

	class Renderer {
	public:
		Renderer()
		:objects{}
		{

		};
		~Renderer() = default;

	public:
		Renderer& attatchObject(RenderObject* obj);
		void render(const RenderingContext& ctx);

	private:
		std::vector<RenderObject*> objects;
	};
};