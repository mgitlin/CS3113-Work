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
#include "Enemy.h"
#include <vector>

enum GameState { MAIN_MENU, GAME_LEVEL, GAME_OVER };
int state;

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

GLuint LoadBackground() {
	SDL_Surface *surface = IMG_Load(RESOURCE_FOLDER"black.png");
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SDL_FreeSurface(surface);
	return textureID;
}

void DrawBackground(ShaderProgram* program, GLuint texID) {
	float vertices[] = { -1.2f, -1.2f, 1.2f, 1.2f, -1.2f, 1.2f, 1.2f, 1.2f, -1.2f, -1.2f, 1.2f, -1.2f };
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

void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 500, 600);

	Matrix modelMatrixMain;
	Matrix modelMatrixOver;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.0, 1.0, -1.2f, 1.2f, -1.0f, 1.0f);

	std::vector<Enemy> enemies;
	int enemies_alive;
	// Sprite loading
	GLuint spriteSheet = LoadTexture(RESOURCE_FOLDER"sheet.png");
	GLuint fontSheet = LoadTexture(RESOURCE_FOLDER"font1.png");
	SheetSprite playerSprite(program, spriteSheet, 211.0f / 1024.0f, 941.0f / 1024.0f, 99.0f / 1024.0f, 75.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite playerShotSprite(program, spriteSheet, 856.0f / 1024.0f, 421.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 0.1, GL_CLAMP);
	SheetSprite enemySprite1(program, spriteSheet, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite enemySprite2(program, spriteSheet, 143.0f / 1024.0f, 293.0f / 1024.0f, 104.0f / 1024.0f, 84.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite enemySprite3(program, spriteSheet, 224.0f / 1024.0f, 496.0f / 1024.0f, 103.0f / 1024.0f, 84.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite enemySprite4(program, spriteSheet, 520.0f / 1024.0f, 577.0f / 1024.0f, 82.0f / 1024.0f, 84.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite enemyShotSprite(program, spriteSheet, 856.0f / 1024.0f, 983.0f / 1024.0f, 9.0f / 1024.0f, 37.0f / 1024.0f, 0.1, GL_CLAMP);
	GLuint backgroundTexture = LoadBackground();
	// Unit Generation
	Player player(program, playerSprite, playerShotSprite, 0.0f, -0.95f, 1.0f);
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 6; j++) {
			if (i == 0){
				Enemy enemy(program, enemySprite1, enemyShotSprite, -0.66f + (j * 0.28f) - 0.04f, 1.0f);
				enemies.push_back(enemy);
			}
			else if (i == 1){
				Enemy enemy(program, enemySprite2, enemyShotSprite, -0.66f + (j * 0.28f) - 0.04f, 0.65f);
				enemies.push_back(enemy);
			}
			else if (i == 2){
				Enemy enemy(program, enemySprite3, enemyShotSprite, -0.66f + (j * 0.28f) - 0.04f, 0.3f);
				enemies.push_back(enemy);
			}
			else if (i = 3){
				Enemy enemy(program, enemySprite4, enemyShotSprite, -0.66f + (j * 0.28f) - 0.04f, -0.05f);
				enemies.push_back(enemy);
			}
		}
	}
	enemies_alive = enemies.size();

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

		switch (state)
		{
		case MAIN_MENU:
			program->setModelMatrix(modelMatrixMain);
			modelMatrixMain.identity();
			modelMatrixMain.Translate(-0.9f, 0.0f, 0.0f);
			//DrawBackground(program, backgroundTexture);
			DrawText(program, fontSheet, "Press enter to start!", 0.075f, 0.0005f);
			if (keys[SDL_SCANCODE_RETURN]) { // Press enter to get into game level
				state = GAME_LEVEL;
			}	
			break;
		case GAME_LEVEL:
			if (player.alive && enemies_alive > 0){ // Once player/all enemies die go to game over screen
				player.Update();
				for (int i = 0; i < 24; i++) enemies[i].Update(player.x_pos, player.sprite, player.bullet, player.alive, enemies_alive);
			}
			else {
				state = GAME_OVER;
			}
			break;
		case GAME_OVER:
			program->setModelMatrix(modelMatrixOver);
			modelMatrixOver.identity();
			modelMatrixOver.Translate(-0.9f, 0.0f, 0.0f);
			//DrawBackground(program, backgroundTexture);
			DrawText(program, fontSheet, "Thanks for playing!\nPress enter to quit!", 0.075f, 0.0005f);
			if (keys[SDL_SCANCODE_RETURN]) {
				done = true;
			}
			break;
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}