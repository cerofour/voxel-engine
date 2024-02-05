#pragma once

#include <vector>
#include <tuple>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

namespace dlb {

	using uint = unsigned int;

	class ShaderProgram {
	public:

		ShaderProgram() = default;
		~ShaderProgram() = default;

		ShaderProgram(uint id) {
			this->shaderProgramId = id;
		}

		ShaderProgram(const ShaderProgram& rhs) = default;
		ShaderProgram(ShaderProgram&& rhs) = default;

		ShaderProgram& operator=(const ShaderProgram& rhs) = default;
		ShaderProgram& operator=(ShaderProgram&& rhs) = default;

	public:
		void use() {
			glUseProgram(this->shaderProgramId);
		}

		uint getProgramId() { return this->shaderProgramId; }

		void setUniform(const std::string& uniformName, int x) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniform1i(loc, x);
		}

		void setUniform1f(const std::string& uniformName, float x) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniform1f(loc, x);
		}

		void setUniform2f(const std::string& uniformName, float x, float y) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniform2f(loc, x, y);
		}

		void setUniform4f(const std::string& uniformName, float x1, float x2, float x3, float x4) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniform4f(loc, x1, x2, x3, x4);
		}

		void setUniform(const std::string& uniformName, const glm::vec3& o) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniform3f(loc, o.x, o.y, o.z);
		}
		
		void setUniform(const std::string& uniformName, glm::mat4& mat4) {
			const auto loc = glGetUniformLocation(this->shaderProgramId, uniformName.c_str());
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat4));
		}
		
	private:
		uint shaderProgramId;
	};

	class ShaderProgramBuilder {
	public:
		ShaderProgramBuilder() {  }

	public:
		ShaderProgram build() {
			auto programId = glCreateProgram();

			for (const auto& shader : this->shaders)
				glAttachShader(programId, shader);

			glLinkProgram(programId);

			{ // make success only available to this scope.
				int success = 0;
				glGetProgramiv(programId, GL_LINK_STATUS, &success);
				if (!success) {
					char infoLog[512] = {};
					glGetProgramInfoLog(programId, 512, NULL, infoLog);
					std::cerr << "shader compilation failed <" << infoLog << ">" << std::endl;
				}
			}

			for (const auto& shader : this->shaders)
				glDeleteShader(shader);

			return programId;
		}

		ShaderProgramBuilder& shaderFromFile(const char* filePath) {
			unsigned int shaderId = glCreateShader(GL_VERTEX_SHADER);
			std::string code = this->getFileContents(filePath);
			this->compileShader(shaderId, code);
			return *this;
		}

		ShaderProgramBuilder& fragmentFromFile(const char* filePath) {
			unsigned int fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
			std::string code = this->getFileContents(filePath);
			this->compileShader(fragmentId, code);
			return *this;
		}

		ShaderProgramBuilder& shaderFromString(std::string& str) {
			unsigned int shaderId = glCreateShader(GL_VERTEX_SHADER);
			this->compileShader(shaderId, str);
		}

		ShaderProgramBuilder& fragmentFromString(std::string& str) {
			unsigned int shaderId = glCreateShader(GL_FRAGMENT_SHADER);
			this->compileShader(shaderId, str);
		}
	
	private:
		void compileShader(uint shaderId, std::string& _code) {
			if (_code.length() == 0)
				return;

			const char* code = _code.c_str();
			glShaderSource(shaderId, 1, &code, NULL);
			glCompileShader(shaderId);

			int success = 0;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[512] = {};
				glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
				std::cerr << "fragment compilation failed <" << infoLog << ">" << std::endl;
			}

			this->shaders.push_back(shaderId);
		}

	private:
		std::string getFileContents(const char* filepath) {
			std::string fileContents;
			std::ifstream inputStream;
			inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				// open files
				inputStream.open(filepath);
				std::stringstream inputSStream;
				// read file's buffer contents into streams
				inputSStream << inputStream.rdbuf();
				// close file handlers
				inputStream.close();
				// convert stream into string
				fileContents = inputSStream.str();
			}
			catch (std::ifstream::failure e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
			}

			return fileContents;
		}
	private:
		std::vector<uint> shaders;
	};
}