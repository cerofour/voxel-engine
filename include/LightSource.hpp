#pragma once

#include "Rendering.hpp"
#include "TextureLoader.h"

class LightSource {
public:
	LightSource(const glm::vec3& pos, const glm::vec3& color = glm::vec3(1.0f));
public:

	// @brief Sets all the uniforms this->affectedObjectsSP such that
	//		lighting calculations can be performed using that shader program.
	virtual void setLightingUniforms(const rendering::RenderingContext& ctx) = 0;

	void appendAffectedObjectSP(dlb::ShaderProgram* sp) {
		this->affectedObjectsSP.push_back(sp);
	}

	glm::vec3& getPosition() {
		return this->position;
	}

	glm::vec3& getColor() {
		return this->color;
	}

protected:
	glm::vec3 position;
	glm::vec3 color;
	std::vector<dlb::ShaderProgram*> affectedObjectsSP;
};

class CubeLight : public rendering::RenderObject, public LightSource {
public:
	CubeLight(const glm::vec3& pos);

public:
	void preRender();
	void setLightingUniforms(const rendering::RenderingContext& ctx) override;
	void render(const rendering::RenderingContext& ctx);

public:
	void dontMove() {
		this->move = false;
	}

	void doMove() {
		this->move = true;
	}

	void setMove(bool v) {
		this->move = v;
	}

private:
	dlb::ShaderProgram shaderProgram;
	dlb::TextureLoader textureLoader;
	rendering::MultipleBufferVAO vao;
	bool doPreRender;
	bool move;
};