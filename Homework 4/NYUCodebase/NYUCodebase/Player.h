#pragma once

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
#include "SheetSprite.h"

class Player {
public:
	Player();
	Player(SheetSprite sprite);
	void Update(ShaderProgram* program, float elapsed);
	void Render(ShaderProgram* program);
	bool collidesWith();
	float lerp(float v0, float v1, float t);
	const Uint8 *keys;
	ShaderProgram* program;
	
	Matrix modelMatrix;
	SheetSprite sprite;

	float x;
	float y;
	float width;
	float height;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;

	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;
};
