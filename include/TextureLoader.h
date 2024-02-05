#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>

namespace dlb {
	class TextureLoader {
	public:
		TextureLoader()
			:textures{} {
		}

	public:
		void checkGLError(const char* functionName) {
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

		TextureLoader& loadTexture(const std::string& filePath) {
			if (this->textures.size() == 16)
				return *this;

			int width, height, nrChannels;	
			stbi_set_flip_vertically_on_load(true);
			unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

			unsigned int tex = 0;

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);

			GLenum format = GL_RGB;

			if (this->endsWith(filePath, ".png"))
				format = GL_RGBA;

			if (data) {
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				// default parameters
				// set the texture wrapping/filtering options (on the currently bound texture object)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else {
				std::cerr << "failed to load texture" << std::endl;
			}
			stbi_image_free(data);

			this->textures.push_back(tex);

			return *this;
		}

		/*
		* Sets parameters for the LAST texture loaded (binds the texture and then sets the parameters).
		*/
		TextureLoader& setTexParameterI(GLenum parameter, GLint val) {
			glBindTexture(GL_TEXTURE_2D, this->textures.back());
			glTexParameteri(GL_TEXTURE_2D, parameter, val);
			return *this;
		}

		void enableTextures() {
			GLenum texNo = GL_TEXTURE0;
			for (const auto& tex : this->textures) {
				glActiveTexture(texNo++);
				glBindTexture(GL_TEXTURE_2D, tex);
			}
		}

	public:
		void bindLast() {
			glBindTexture(GL_TEXTURE_2D, this->textures.back());
		}

	private:
		bool endsWith(const std::string& str, const std::string& suffix) {
			if (str.length() < suffix.length())
				return false;
			return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
		}

	private:
		std::vector<unsigned int> textures;
	};
}