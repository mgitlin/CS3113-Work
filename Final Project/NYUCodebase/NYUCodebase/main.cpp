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
#include "Entity.h"
#include "Level.h"

#include <vector>

enum GameState { MAIN_MENU, GAME_LEVEL_1, GAME_LEVEL_2, GAME_LEVEL_3, GAME_OVER };
int state;

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

void RenderBG(ShaderProgram *program, GLint img){
	glBindTexture(GL_TEXTURE_2D, img);
	GLfloat texCoords[] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
	float vertices[] = { -10.0f, -7.5f, 10.0f, 7.5f, -10.0f, 7.5f, 10.0f, 7.5f, -10.0f, -7.5f, 10.0f, -7.5f };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
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

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Matrix modelMatrix;
	Matrix textMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
	
	std::vector<Entity> entities;

	int lastWinner;

	// Load player sprites
	GLint paddleBlue = LoadTextureClamp(RESOURCE_FOLDER"assets\\paddleBlu.png");
	GLint paddleRed = LoadTextureClamp(RESOURCE_FOLDER"assets\\paddleRed.png");
	// Load item sprites
	GLint coinCopperSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\coinCopper.png");
	GLint coinSilverSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\coinSilver.png");
	GLint coinGoldSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\coinGold.png");
	GLint coinPlatinumSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\coinPlatinum.png");
	GLint starCopperSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\starCopper.png");
	GLint starSilverSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\starSilver.png");
	GLint starGoldSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\starGold.png");
	GLint starPlatinumSprite = LoadTextureClamp(RESOURCE_FOLDER"assets\\starPlatinum.png");
	// Load font
	GLuint fontSheet = LoadTextureClamp(RESOURCE_FOLDER"assets\\font1.png");
	// Load level bg
	GLint mainBG = LoadTextureRepeat(RESOURCE_FOLDER"assets\\bg_main.png");
	GLint levelBG = LoadTextureRepeat(RESOURCE_FOLDER"assets\\bg_level.png");
	GLint overBG = LoadTextureRepeat(RESOURCE_FOLDER"assets\\bg_over.png");

	// Load music
	Mix_Music *music1;
	music1 = Mix_LoadMUS(RESOURCE_FOLDER"assets\\Im Free.mp3");
	Mix_Music *music2;
	music2 = Mix_LoadMUS(RESOURCE_FOLDER"assets\\Lagrima.mp3");
	Mix_Music *music3;
	music3 = Mix_LoadMUS(RESOURCE_FOLDER"assets\\The Old Song.mp3");
	Mix_Music *music4;
	music4 = Mix_LoadMUS(RESOURCE_FOLDER"assets\\FASE.mp3");
	// Load sounds
	Mix_Chunk* itemHit;
	itemHit = Mix_LoadWAV(RESOURCE_FOLDER"hit.wav");
	Mix_Chunk* buttonPress;
	buttonPress = Mix_LoadWAV(RESOURCE_FOLDER"Menu Selection Click.wav");


	// Initialize players
	Entity playerBlue(ENTITY_PLAYER, paddleBlue, fontSheet, SDL_SCANCODE_A, SDL_SCANCODE_D, -5.0f);
	Entity playerRed(ENTITY_PLAYER, paddleRed, fontSheet, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, 5.0f);

	Entity coinCopper(ENTITY_ITEM, coinCopperSprite, fontSheet, 10, itemHit);
	Entity coinSilver(ENTITY_ITEM, coinSilverSprite, fontSheet, 25, itemHit);
	Entity coinGold(ENTITY_ITEM, coinGoldSprite, fontSheet, 50, itemHit);
	Entity coinPlatinum(ENTITY_ITEM, coinPlatinumSprite, fontSheet, 100, itemHit);
	Entity starCopper(ENTITY_ITEM, starCopperSprite, fontSheet, 100, itemHit);
	Entity starSilver(ENTITY_ITEM, starSilverSprite, fontSheet, 250, itemHit);
	Entity starGold(ENTITY_ITEM, starGoldSprite, fontSheet, 500, itemHit);
	Entity starPlatinum(ENTITY_ITEM, starPlatinumSprite, fontSheet, 1000, itemHit);

	entities.push_back(playerBlue);
	entities.push_back(playerRed);
	entities.push_back(coinCopper);
	entities.push_back(coinSilver);
	entities.push_back(coinGold);
	entities.push_back(coinPlatinum);
	entities.push_back(starCopper);
	entities.push_back(starSilver);
	entities.push_back(starGold);
	entities.push_back(starPlatinum);

	Level level1(LEVEL_FREEPLAY, fontSheet, levelBG, music2, entities);
	Level level2(LEVEL_CLOCKED, fontSheet, levelBG, music4, entities);
	Level level3(LEVEL_SCORED, fontSheet, levelBG, music1, entities);

	Mix_PlayMusic(music3, -1);

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

		switch (state)
		{
		case MAIN_MENU:
			RenderBG(program, mainBG);
			if (keys[SDL_SCANCODE_1]) { // Press 1 to get into game level
				state = GAME_LEVEL_1;
				Mix_PlayChannel(-1, buttonPress, 0);
				Mix_PlayMusic(level1.getBgm(), -1);
			}
			if (keys[SDL_SCANCODE_2]) { // Press 2 to get into game level
				state = GAME_LEVEL_2;
				Mix_PlayChannel(-1, buttonPress, 0);
				Mix_PlayMusic(level2.getBgm(), -1);
			}
			if (keys[SDL_SCANCODE_3]) { // Press 3 to get into game level
				state = GAME_LEVEL_3;
				Mix_PlayChannel(-1, buttonPress, 0);
				Mix_PlayMusic(level3.getBgm(), -1);
			}
			if (keys[SDL_SCANCODE_ESCAPE]){
				done = true;
			}
			break;
		case GAME_LEVEL_1:
			if (!level1.isComplete()){
				level1.Update();
				level1.FixedUpdate();
				level1.Render(program);
				level1.RenderText(program);
			}
			else {
				lastWinner = level1.getWinner();
				state = MAIN_MENU;
				level1.reset();
				Mix_PlayMusic(music3, -1);
			}
			break;
		case GAME_LEVEL_2:
			if (!level2.isComplete()){
				level2.Update();
				level2.FixedUpdate();
				level2.Render(program);
				level2.RenderText(program);
			}
			else {
				lastWinner = level2.getWinner();
				state = GAME_OVER;
				level2.reset();
				Mix_PlayMusic(music3, -1);
			}
			break;
		case GAME_LEVEL_3:
			if (!level3.isComplete()){
				level3.Update();
				level3.FixedUpdate();
				level3.Render(program);
				level3.RenderText(program);
			}
			else {
				lastWinner = level3.getWinner();
				state = GAME_OVER;
				level3.reset();
				Mix_PlayMusic(music3, -1);
			}
			break;
		case GAME_OVER:
			RenderBG(program, overBG);
			program->setModelMatrix(textMatrix);
			textMatrix.identity();
			textMatrix.Translate(-7.5f, 0.0f, 0.0f);
			if (lastWinner == 0)
				DrawText(program, fontSheet, "Blue Player wins!", 1.0f, 0.005f);
			else if (lastWinner == 1)
				DrawText(program, fontSheet, "Red Player wins!", 1.0f, 0.005f);
			if (keys[SDL_SCANCODE_RETURN]) {
				level1.reset();
				level2.reset();
				level3.reset();
				state = MAIN_MENU;
				Mix_PlayChannel(-1, buttonPress, 0);
			}
			else if (keys[SDL_SCANCODE_ESCAPE]) {
				done = true;
			}
			break;
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}