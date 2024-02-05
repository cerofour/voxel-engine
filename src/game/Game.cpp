#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <math.h>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <Windows.h>

#include "ShaderCreation.hpp"
#include "TextureLoader.h"
#include "Camera.hpp"
#include "ChunkMesh2.hpp"
#include "Rendering.hpp"
#include "LightSource.hpp"

#include "Application.hpp"

const glm::vec4 SKYCOLOR{ 0.0f, 0.0f, 0.0f, 1.0 };

// Overload the << operator for std::ostream and std::vector<float>
std::ostream& operator<<(std::ostream& os, const std::vector<float>& vec) {
	os << "[ ";

	int i = 0;
	int lineno = 0;

	for (const auto& element : vec) {

		if (i++ % 3 == 0) {
			lineno++;
			os << std::endl << lineno << " ";
		}

		os << element << " ";
	}
	os << "]";
	return os;
}

int main(void) {
	auto app = Application::getInstance();

	app->onInit();

	std::unique_ptr<CubeLight> cl = std::make_unique<CubeLight>(glm::vec3(1.0f, -10.0f, 20.0f));
	std::vector<std::unique_ptr<ChunkMesh2>> chunks{};
	rendering::Renderer renderer{};
	renderer.attatchObject(cl.get());

	for (int x = 0; x < WORLDSIZE; x++) {
		for (int z = 0; z < WORLDSIZE; z++) {
			chunks.push_back(
				std::make_unique<ChunkMesh2>(glm::vec3(x, 0.0f, z))
			);
			chunks.back()->setId(chunks.size() - 1);
			chunks.back()->generateChunk();
			chunks.back()->generateMesh();
			renderer.attatchObject(chunks.back().get());
		}
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float lastTime = glfwGetTime();
	unsigned int frameCount = 0;

	app->getCamera().toggleFreeCamera();

	/*
	* RENDER LOOP
	*/
	while (!glfwWindowShouldClose(app->getWindow())) {
		/*
		* Input processing
		*/
		app->onUpdate();

		cl->setMove(app->doMoveLight());

		glClearColor(SKYCOLOR.x, SKYCOLOR.y, SKYCOLOR.z, SKYCOLOR.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		//texLoader.enableTextures();

		auto currentFrame = static_cast<float>(glfwGetTime());
		app->getDeltaTime() = currentFrame - app->getLastFrame();
		app->getLastFrame() = currentFrame;

		frameCount++;

		if (currentFrame - lastTime >= 1.0f) {
			std::cout << "FPS " << (float)frameCount << std::endl;
			frameCount = 0;
			lastTime += 1.0f;
		}

		auto ctx = app->getContext();
		ctx.lightSource = cl.get();

		renderer.render(ctx);

		glfwSwapBuffers(app->getWindow());
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}