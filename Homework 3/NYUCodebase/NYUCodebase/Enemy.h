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
#include "Bullet.h"


class Enemy {
public:
	Enemy();
	Enemy(ShaderProgram* program, SheetSprite sprite, SheetSprite shotSprite, float x_pos, float y_pos);
	void Update(const float & player_x_pos, const SheetSprite & player_sprite, Bullet & player_bullet, bool & player_alive, int & enemies_alive);
	const Uint8 *keys;
	ShaderProgram* program;
	Matrix modelMatrix;
	SheetSprite sprite;
	SheetSprite shotSprite;
	float lastFrameTicks;
	float x_pos;
	float y_pos;
	float move_speed;
	bool alive;
	int bulletIndex;
	Bullet bullet;

};