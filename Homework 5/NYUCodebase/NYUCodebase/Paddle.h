#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

class Paddle {
public:
	Paddle();
	Paddle(float x, float height, float width, float speed, SDL_Scancode upKey, SDL_Scancode downKey);
	void Update(float elapsed);
	void Render(ShaderProgram* program);

	Matrix modelMatrix;
	const Uint8 *keys;
	SDL_Scancode upKey;
	SDL_Scancode downKey;
	float x;
	float y;
	float height;
	float width;
	float speed;
};