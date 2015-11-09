#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "Matrix.h"
#include "Level.h"

// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.03333333f
#define MAX_TIMESTEPS 6

const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SDL_FreeSurface(surface);
	return textureID;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Final Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, 640, 480);

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	float timeLeftOver = 0.0f;

	// Load backgrounds
	GLint bg_forest = LoadTexture(RESOURCE_FOLDER"forest.png");
	GLint bg_mountain = LoadTexture(RESOURCE_FOLDER"mountain.png");

	// Load tileset
	GLint tileset = LoadTexture(RESOURCE_FOLDER"tileset.png");

	Level level1("level1.txt", tileset, ParallaxBackground(bg_forest));
//	Level level1("level1.txt", tileset, ParallaxBackground(bg_mountain));

	float x = 0.0f;

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program->setModelMatrix(modelMatrix);
		program->setViewMatrix(viewMatrix);
		program->setProjectionMatrix(projectionMatrix);
		glUseProgram(program->programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		float fixedElapsed = elapsed + timeLeftOver;
		if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
			fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
		}
		while (fixedElapsed >= FIXED_TIMESTEP) {
			fixedElapsed -= FIXED_TIMESTEP;
		}
		timeLeftOver = fixedElapsed;

		if (keys[SDL_SCANCODE_RIGHT]) { 
			x -= fixedElapsed * 0.75;
		} // move right

		viewMatrix.identity();
		viewMatrix.Translate(x, 0.0f, 0.0f);

		level1.Render(program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}