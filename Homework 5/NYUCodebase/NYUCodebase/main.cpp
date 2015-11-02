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
#include "Paddle.h"
#include "Ball.h"

// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.01666667f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glViewport(0, 0, 640, 360); // Half of 720p

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	float timeLeftOver = 0.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	Mix_Music *music;
	music = Mix_LoadMUS("gba1complete.mp3");

	Paddle leftPaddle(-1.652f, 0.4f, 0.05f, 0.05f, SDL_SCANCODE_W, SDL_SCANCODE_S);
	Paddle rightPaddle(1.652f, 0.4f, 0.05f, 0.05f, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
	Mix_Chunk* ballHit;
	Mix_Chunk* ballDie;
	ballHit = Mix_LoadWAV("hit.wav");
	ballDie = Mix_LoadWAV("die.wav");
	Ball ball(0.05f, 0.05f, ballHit, ballDie);

	Mix_PlayMusic(music, -1);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				Mix_FreeChunk(ballHit);
				Mix_FreeChunk(ballDie);
				Mix_FreeMusic(music);
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

		leftPaddle.Update(fixedElapsed);
		rightPaddle.Update(fixedElapsed);
		ball.Update(fixedElapsed, leftPaddle, rightPaddle);

		leftPaddle.Render(program);
		rightPaddle.Render(program);
		ball.Render(program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}