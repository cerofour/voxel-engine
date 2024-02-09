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

#include "Types.hpp"
#include "ShaderCreation.hpp"
#include "TextureLoader.h"
#include "Camera.hpp"
#include "ChunkMesh2.hpp"
#include "Rendering.hpp"
#include "LightSource.hpp"

#include "Application.hpp"

const glm::vec4 SKYCOLOR{ 0.21, 0.78, 0.95, 1.0 };


int main(void) {
	auto app = Application::getInstance();

	app->onInit();

	std::unique_ptr<CubeLight> cl = std::make_unique<CubeLight>(glm::vec3(1.0f, 80.0f, 64.0f));
	rendering::Renderer renderer{};
	renderer.attatchObject(cl.get());

	// dont forget to delete this
	Array3D<ChunkMesh2*, WORLDSIZE, WORLDSIZE, WORLDSIZE> world{};

	for (int x = 0; x < WORLDSIZE; x++) {
		for (int y = 0; y < WORLDSIZE; y++) {
			for (int z = 0; z < WORLDSIZE; z++) {
				auto chunk = new ChunkMesh2(glm::vec3((float)x, (float)y, (float)z));
				chunk->generateChunk();
				world.at(x, y, z) = chunk;
				renderer.attatchObject(chunk);
			}
		}
	}


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
			std::cout << "FPS " << (float)frameCount << '\n';
			frameCount = 0;
			lastTime += 1.0f;
		}

		auto px = app->getCamera().getPosition().x;
		auto py = app->getCamera().getPosition().y;
		auto pz = app->getCamera().getPosition().z;

		auto ctx = app->getContext();
		ctx.lightSource = cl.get();
		ctx.world = &world;

		renderer.render(ctx);

		glfwSwapBuffers(app->getWindow());
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}