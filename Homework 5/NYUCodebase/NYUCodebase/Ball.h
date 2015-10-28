#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Paddle.h"

class Ball {
public:
	Ball();
	Ball(float size, float speed);
	void Update(float elapsed, Paddle& left, Paddle& right);
	void Render(ShaderProgram* program);
	int randDir();

	Matrix modelMatrix;
	const Uint8 *keys;

	float size;
	float x;
	float y;
	float dir_x = 0.0f;
	float dir_y = 0.0f;
	float speed = 2.0f;
};