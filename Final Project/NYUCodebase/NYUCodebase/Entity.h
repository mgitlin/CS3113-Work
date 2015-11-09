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

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY};

class Entity {
public:
	Entity(); // Default
//	Entity();
	~Entity();

	void Update(float elapsed);
	void FixedUpdate(float fixedElapsed);
	void Render(ShaderProgram *program);
	bool collidesWith(Entity *other);

private:

	float x;
	float y;
	float width;
	float height;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;

	bool isStatic;
	EntityType type;

	bool collidedTop;
	bool collidedRight;
	bool collidedBottom;
	bool collidedLeft;
};