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
#include "SheetSprite.h"
#include "Player.h"
#include "Level.h"

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
	SDL_FreeSurface(surface);
	return textureID;
}

GLuint LoadBackground(const char *image_path) {
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

void DrawBackground(ShaderProgram* program, GLuint texID) {
	float vertices[] = { -4.0f, -.75f, 4.0f, 1.0f, -4.0f, 1.0f, 4.0f, 1.0f, -4.0f, -.75f, 4.0f, -.75f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, texID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, 480, 480);

	Matrix modelMatrixFarBG;
	Matrix modelMatrixNearBG;
	Matrix modelMatrixLevel;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	// Sprite loading
	GLuint tileSet = LoadTexture(RESOURCE_FOLDER"tileset.png");
	GLuint farBackground = LoadBackground(RESOURCE_FOLDER"far-background.png");
	GLuint nearBackground = LoadBackground(RESOURCE_FOLDER"near-background.png");
	GLuint playerTex = LoadTexture(RESOURCE_FOLDER"000.png");
	SheetSprite playerSprite(playerTex, 0.0f, 0.0f, 1.0f, 1.0f, 0.25f);

	// Level loading
	Level level("level.txt", tileSet);

	float nearBG = 3.5f;
	float farBG = 3.5f;
	float viewY = 1.0f;
	float lastFrameTicks = 0.0f;

	// Unit Generation
	Player player(playerSprite);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program->setViewMatrix(viewMatrix);
		program->setProjectionMatrix(projectionMatrix);
		glUseProgram(program->programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (keys[SDL_SCANCODE_RIGHT]) { farBG -= elapsed * 0.05f; nearBG -= elapsed * 0.15f; }
		else if (keys[SDL_SCANCODE_LEFT]){ farBG += elapsed * 0.05f; nearBG += elapsed * 0.15f; }

		program->setModelMatrix(viewMatrix);
		viewMatrix.identity();
		viewMatrix.Translate(-player.x, viewY, 0.0f);

		program->setModelMatrix(modelMatrixFarBG);
		
		modelMatrixFarBG.identity();
		modelMatrixFarBG.Translate(farBG, -1.0f, 0.0f);
		DrawBackground(program, farBackground);

		program->setModelMatrix(modelMatrixNearBG);
		modelMatrixNearBG.identity();
		modelMatrixNearBG.Translate(nearBG, -1.0f, 0.0f);
		DrawBackground(program, nearBackground);
		
		program->setModelMatrix(modelMatrixLevel);
		level.Render(program);
		player.Update(program, elapsed);
		player.Render(program);

		level.Update(player);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}