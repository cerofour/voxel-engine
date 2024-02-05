#include "LightSource.hpp"
#include "Config.hpp"
#include "ShaderCreation.hpp"

CubeLight::CubeLight(const glm::vec3& pos)
	:LightSource{ pos },
	vao {},
	shaderProgram{},
	doPreRender{ true },
	move{ true } {

	dlb::ShaderProgramBuilder spb{};
	this->shaderProgram = std::move(spb
		.shaderFromFile(SHADERSDIR "/lightCube.vert")
		.fragmentFromFile(SHADERSDIR "/lightCube.frag")
		.build());

	this->textureLoader
		.loadTexture(TEXTUREDIR "/awesomeface.png");
}

void CubeLight::preRender() {
	const float cubeVertices[] = {
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
	const float textureCoords[] = {
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

	this->doPreRender = false;

	this->vao
		.use()
		.addVertexAttribute(cubeVertices, sizeof(cubeVertices) / sizeof(float), 3, 0, GL_FALSE)
		.addVertexAttribute(textureCoords, sizeof(textureCoords) / sizeof(float), 2, 1, GL_FALSE);
	this->vao.verticesN = 36;
}

void CubeLight::render(const rendering::RenderingContext& ctx) {
	if (this->doPreRender)
		this->preRender();

	auto model = glm::mat4(1.0f);

	this->position = glm::vec3(
		this->position.x * (int)(!this->move) + (CHUNKSIZE * WORLDSIZE * glm::cos(glfwGetTime())) * (int)this->move,
		this->position.y,
		this->position.z * (int)(!this->move) + (CHUNKSIZE * WORLDSIZE * glm::sin(glfwGetTime())) * (int)this->move);

	model = glm::translate(model, this->position);
	model = glm::scale(model, glm::vec3(.4f));

	this->shaderProgram.use();
	this->textureLoader.enableTextures();
	this->shaderProgram.setUniform("texture0", 0);

	auto MVP = ctx.camera.getProjectionMatrix()
		* ctx.camera.getViewMatrix()
		* model;

	this->shaderProgram.setUniform("MVP", MVP);
	this->vao.draw();

	//this->setLightingUniforms(ctx);
}

void CubeLight::setLightingUniforms(const rendering::RenderingContext& ctx) {
	// WARNING: Shader dependent code. This only works for the "chunk.frag" fragment shader.
	// for any other shader program rewrite this method.
	for (const auto sp : this->affectedObjectsSP) {
		sp->use();
	}
}

LightSource::LightSource(const glm::vec3& pos, const glm::vec3& color)
	:position{ pos },
	color {color},
	affectedObjectsSP{} { }