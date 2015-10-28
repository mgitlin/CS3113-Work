#include "Player.h"

Player::Player() : keys(SDL_GetKeyboardState(NULL)), x(1.2f), y(1.0f), velocity_x(0.0f), velocity_y(0.0f), acceleration_x(1.0f), acceleration_y(-0.5f) {};
Player::Player(SheetSprite sprite)
	: keys(SDL_GetKeyboardState(NULL)), sprite(sprite), x(1.2f), y(-1.37f), velocity_x(0.0f), velocity_y(0.0f), acceleration_x(1.0f), acceleration_y(-0.75f), width(sprite.size), height(sprite.size)
{};

void Player::Update(ShaderProgram* program, float elapsed) {
		velocity_x = lerp(velocity_x, 0.0f, elapsed * 0.5f);
		velocity_y = lerp(velocity_y, 0.0f, elapsed * 0.5f);

		velocity_x += acceleration_x * elapsed;
		velocity_y += acceleration_y * elapsed; 

		if (keys[SDL_SCANCODE_RIGHT]) { x += elapsed * 1.0f; if (keys[SDL_SCANCODE_SPACE]) { velocity_y = 1.0f; } } // move right
		else if (keys[SDL_SCANCODE_LEFT]){ x -= elapsed * 1.0f; if (keys[SDL_SCANCODE_SPACE]) { velocity_y = 1.0f; } } // move left
		else if (keys[SDL_SCANCODE_SPACE]) { velocity_y = 1.0f; }

		//x += velocity_x * elapsed;
		y += velocity_y * elapsed;

		program->setModelMatrix(modelMatrix);

		modelMatrix.identity();
		modelMatrix.Translate(x, y, 0.0f);
}

void Player::Render(ShaderProgram* program){
	sprite.Draw(program);
}

float Player::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}