#pragma once
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

#include <vector>
using namespace std;

// 10 FPS (1.0f/10.0f)
#define FIXED_TIMESTEP 0.1f
#define MAX_TIMESTEPS 1

enum LevelType{LEVEL_FREEPLAY, LEVEL_CLOCKED, LEVEL_SCORED};

class Level {
public:
	//Default
	Level();

	Level(LevelType type, GLint font, GLint bg, Mix_Music *bgm, vector<Entity> entitiesVec);

	~Level();

	void Update();
	void FixedUpdate();
	void Render(ShaderProgram *program);
	void RenderText(ShaderProgram *program);
	void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing);

	void reset();
	int getWinner();
	bool isComplete();
	Mix_Music* getBgm();

private:

	GLint bg;
	Mix_Music *bgm;
	
	LevelType type;
	GLint font;
	float clock;
	vector<Entity> entities;
	bool complete;
	int winner;

	float lastFrameTicks;
	float timeLeftOver;
	float elapsed;
	float fixedElapsed;

	const Uint8 *keys;
	Matrix modelMatrix;
	Matrix textMatrix;
	Matrix textMatrix2;
};