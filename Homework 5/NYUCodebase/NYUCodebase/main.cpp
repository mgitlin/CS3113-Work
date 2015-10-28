#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "Matrix.h"
#include "Paddle.h"
#include "Ball.h"

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glViewport(0, 0, 640, 360); // Half of 720p

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	float lastFrameTicks = 0.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	Paddle leftPaddle(-1.652f, 0.4f, 0.05f, 1.0f, SDL_SCANCODE_W, SDL_SCANCODE_S);
	Paddle rightPaddle(1.652f, 0.4f, 0.05f, 1.0f, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
	Ball ball(0.05f, 1.5f);

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

		// Left Paddle Begin
		leftPaddle.Update(elapsed);
		leftPaddle.Render(program);
		// Left Paddle End

		// Right Paddle Begin
		rightPaddle.Update(elapsed);
		rightPaddle.Render(program);
		// Right Paddle End

		// Ball Begin
		ball.Update(elapsed, leftPaddle, rightPaddle);
		ball.Render(program);
		// Ball End

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}