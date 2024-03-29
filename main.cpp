#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
ShaderProgram program2;

glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint brachy;
GLuint sword;

float brachy_x = 0.0f;;
float sword_x = 0.0f;

float lastTicks = 0.0f;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Simple 2D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program2.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	brachy = LoadTexture("brachy.png");
	sword = LoadTexture("idekman.png");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	program2.SetProjectionMatrix(projectionMatrix);
	program2.SetViewMatrix(viewMatrix);
	program2.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);
	glUseProgram(program2.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	brachy_x += 1.0f * deltaTime;
	sword_x += 60.0f * deltaTime;
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix = glm::mat4(1.0f);
	program2.SetModelMatrix(modelMatrix);

	float vertices[] = { 5.0, -3.75, 5.0, 3.75, -5.0, 3.75, -5.0, 3.75, -5.0, -3.75, 5.0, -3.75};
	glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program2.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program2.positionAttribute);

	modelMatrix = glm::translate(modelMatrix, glm::vec3(brachy_x, 0.0f, 0.0f));
	program.SetModelMatrix(modelMatrix);

	float brachyVertices[] = { -1.5, -1.5, 1.5, -1.5, 1.5, 1.5, -1.5, -1.5, 1.5, 1.5, -1.5, 1.5 };
	float brachyCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, brachy);

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, brachyVertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, brachyCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	modelMatrix = glm::rotate(modelMatrix, glm::radians(sword_x), glm::vec3(0.0f, 0.0f, 1.0f));
	program.SetModelMatrix(modelMatrix);

	float swordVertices[] = { -1.5, -1.5, 1.5, -1.5, 1.5, 1.5, -1.5, -1.5, 1.5, 1.5, -1.5, 1.5 };
	float swordCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, swordVertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, swordCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, sword);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}