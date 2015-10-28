#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Player.h"

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#define LEVEL_WIDTH 27
#define LEVEL_HEIGHT 8
#define TILE_SIZE 0.25f
#define SPRITE_COUNT_X 2
#define SPRITE_COUNT_Y 2

class Level {
public:
	Level();
	Level(string fileName, GLint textureID);
	bool ReadLayerData(ifstream &stream);
	void Update(Player& player);
	void PrepareVertexData();
	void Render(ShaderProgram* program);
	GLuint LoadTexture(const char *image_path);
	vector<float> vertexData;
	vector<float> texCoordData;
	GLint tileSet;
	ShaderProgram* program;
	Matrix modelMatrix;
	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
};