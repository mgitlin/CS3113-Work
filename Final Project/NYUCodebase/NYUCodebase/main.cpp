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

GLuint LoadTextureClamp(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
}

GLuint LoadTextureRepeat(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
	GLint bg_forest_0 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\forest0.png");
	GLint bg_forest_1 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\forest1.png");
	GLint bg_forest_2 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\forest2.png");
	GLint bg_forest_3 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\forest3.png");
	GLint bg_mountain_0 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\mountain0.png");
	GLint bg_mountain_1 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\mountain1.png");
	GLint bg_mountain_2 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\mountain2.png");
	GLint bg_mountain_3 = LoadTextureRepeat(RESOURCE_FOLDER"levels\\backgrounds\\mountain3.png");

	// Load tileset
	GLint tileset = LoadTextureClamp(RESOURCE_FOLDER"levels\\tileset.png");

	Level level1("", tileset, ParallaxBackground(bg_forest_0, bg_forest_1, bg_forest_2, bg_forest_3));
//	Level level1("", tileset, ParallaxBackground(bg_mountain_0, bg_mountain_1, bg_mountain_2, bg_mountain_3));

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

		/*if (keys[SDL_SCANCODE_RIGHT]) { 
			x -= fixedElapsed * 0.75;
		} // move right
		if (keys[SDL_SCANCODE_LEFT]) {
			x += fixedElapsed * 0.75;
		} // move left
		*/

		level1.FixedUpdate(fixedElapsed);
		level1.Render(program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}