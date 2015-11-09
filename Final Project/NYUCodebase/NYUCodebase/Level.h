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
#include "ParallaxBackground.h"
#include "Entity.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#define LEVEL_WIDTH 40
#define LEVEL_HEIGHT 15
#define TILE_SIZE 1.0f
#define SPRITE_COUNT_X 4
#define SPRITE_COUNT_Y 7

class Level {
public:
	Level(); // Default
	Level(string fileName, GLuint tileset, ParallaxBackground bg);
//	~Level();

	bool ReadLayerData(ifstream &stream);
	void PrepareVertexData();
	void Update(float elapsed, const Entity &player);
	void FixedUpdate(float fixedElapsed, const Entity &player);
	void Render(ShaderProgram *program);

private:
	Mix_Music *bgm;
	ParallaxBackground bg;
	GLuint tileSet;

	vector<float> vertexData;
	vector<float> texCoordData;

	Matrix modelMatrix;
	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];

};