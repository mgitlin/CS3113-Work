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

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
	
	std::vector<Entity> entities;

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
	GLint levelBG = LoadTextureRepeat(RESOURCE_FOLDER"assets\\bg_castle.png");

	Mix_Chunk* itemHit;
	itemHit = Mix_LoadWAV(RESOURCE_FOLDER"hit.wav");

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
	
	Mix_Music *music;
	music = Mix_LoadMUS("gba1complete.mp3");

	Level level1(LEVEL_CLOCKED, "Free Play", fontSheet, levelBG, music, entities);

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

		level1.Update();
		level1.FixedUpdate();
		level1.Render(program);
		level1.RenderText(program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}