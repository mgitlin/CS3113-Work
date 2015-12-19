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

#include <vector>
using namespace std;

enum EntityType {ENTITY_PLAYER, ENTITY_ITEM};

class Entity {
public:
	// Default Constructor
	Entity();
	// Player Constructor
	Entity(EntityType type, GLint sprite, GLint font, SDL_Scancode leftKey, SDL_Scancode rightKey, float start_x);
	// Item Constructor
	Entity(EntityType type, GLint sprite, GLint font, int value, Mix_Chunk* hitSound);

	~Entity();

	void Update(float elapsed);
	void FixedUpdate(float fixedElapsed, Entity *other);
	void Render(ShaderProgram *program);

	void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing);
	void RenderText(ShaderProgram *program);

private:

	float x_pos;
	float start_x;
	float y_pos;
	float width;
	float height;
	float velocity_x;

	int score;
	int value;

	SDL_Scancode leftKey;
	SDL_Scancode rightKey;
	GLint sprite;
	GLint font;

	EntityType type;

	Mix_Chunk* hitSound;

	const Uint8 *keys;
	Matrix modelMatrix;
	Matrix textMatrix;
};