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

SDL_Window* displayWindow;

float randDir() {
	int val = rand() % 2 + 1;
	if (val == 1) return 1.0f;
	return -1.0f;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glViewport(0, 0, 640, 360); // Half of 720p

	Matrix modelMatrixLeft;
	Matrix modelMatrixRight;
	Matrix modelMatrixBall;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	// paddle vars
	float lastFrameTicks = 0.0f;
	float paddleHeight = 0.4f;
	float paddleWidth = 0.05f;
	float paddleLeftY = 0.0f;
	float paddleRightY = 0.0f;

	// ball vars
	float ballLength = 0.05f;
	float ballPosX = 0.0f;
	float ballPosY = 0.0f;
	float ballDirX = 0.0f;
	float ballDirY = 0.0f;
	float ballSpeed = 2.0f;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);
		glUseProgram(program.programID);
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		// Left Paddle Begin
		program.setModelMatrix(modelMatrixLeft);

		if (keys[SDL_SCANCODE_W] && paddleLeftY < 0.8f) { paddleLeftY += elapsed; } // move paddle up
		else if (keys[SDL_SCANCODE_S] && paddleLeftY > -0.8f){ paddleLeftY -= elapsed; } // move paddle down

		float vertices[] = { -paddleWidth / 2, -paddleHeight / 2, -paddleWidth / 2, paddleHeight / 2, paddleWidth / 2, paddleHeight / 2,
			-paddleWidth / 2, -paddleHeight / 2, paddleWidth / 2, -paddleHeight / 2, paddleWidth / 2, paddleHeight / 2 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		modelMatrixLeft.identity();
		modelMatrixLeft.Translate(-1.652f, paddleLeftY, 0.0f);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Left Paddle End

		// Right Paddle Begin
		program.setModelMatrix(modelMatrixRight);
		
		if (keys[SDL_SCANCODE_UP] && paddleRightY < 0.8f) { paddleRightY += elapsed; } // move paddle up
		else if (keys[SDL_SCANCODE_DOWN] && paddleRightY > -0.8f){ paddleRightY -= elapsed; } // move paddle down

		float vertices2[] = { -paddleWidth / 2, -paddleHeight / 2, -paddleWidth / 2, paddleHeight / 2, paddleWidth / 2, paddleHeight / 2,
			-paddleWidth / 2, -paddleHeight / 2, paddleWidth / 2, -paddleHeight / 2, paddleWidth / 2, paddleHeight / 2 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);

		modelMatrixRight.identity();
		modelMatrixRight.Translate(1.652f, paddleRightY, 0.0f);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Right Paddle End

		// Ball Begin
		program.setModelMatrix(modelMatrixBall);
		
		if (keys[SDL_SCANCODE_SPACE] && ballDirX == 0 && ballDirY == 0) { ballDirX = randDir(); ballDirY = randDir(); } // start the ball by hitting the space bar
		ballPosX += elapsed * ballSpeed * ballDirX; 
		ballPosY += elapsed * ballSpeed * ballDirY; 

		if (ballPosY <= -0.975f) { ballDirY = 1.0f; } // ball hitting bottom of the screen
		else if (ballPosY >= 0.975f) { ballDirY = -1.0f; } // ball hitting the top of the screen
		

		if (-1.652f + paddleWidth / 2 >= ballPosX - ballLength / 2 && 
			paddleLeftY - paddleHeight / 2 <= ballPosY - ballLength / 2 &&
			paddleLeftY + paddleHeight / 2 >= ballPosY + ballLength / 2 ) { ballDirX = 1.0f; } // ball hitting left paddle
		
		if (1.652f - paddleWidth / 2 <= ballPosX + ballLength / 2 && 
			paddleRightY - paddleHeight / 2 <= ballPosY - ballLength / 2 && 
			paddleRightY + paddleHeight / 2 >= ballPosY + ballLength / 2) { ballDirX = -1.0f; } // ball hitting right paddle
		
		if (ballPosX >= 1.752f || ballPosX <= -1.752) { ballPosX = 0.0f; ballPosY = 0.0f; ballDirX = 0.0f; ballDirY = 0.0f; 
		paddleLeftY = 0.0f;  paddleRightY = 0.0f; } // reset if somebody misses
		
		float vertices3[] = { -ballLength / 2, -ballLength / 2, ballLength / 2, ballLength / 2, -ballLength / 2, ballLength / 2, 
			-ballLength / 2, -ballLength / 2, ballLength / 2, -ballLength / 2, ballLength / 2, ballLength / 2 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);
		
		modelMatrixBall.identity();
		modelMatrixBall.Translate(ballPosX, ballPosY, 0.0f);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Ball End

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}