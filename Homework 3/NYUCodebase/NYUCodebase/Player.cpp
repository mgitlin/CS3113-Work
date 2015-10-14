#include "Player.h"

Player::Player() : x_pos(0.0f), y_pos(0.0f), move_speed(0.0f) {};
Player::Player(ShaderProgram* program, SheetSprite sprite, SheetSprite shotSprite, float x_pos, float y_pos, float move_speed)
	: program(program), keys(SDL_GetKeyboardState(NULL)), sprite(sprite), shotSprite(shotSprite), x_pos(x_pos), y_pos(y_pos), move_speed(move_speed), 
	bullet(Bullet(program, shotSprite)), alive(true)
{};

void Player::Update() {
	if (alive){
		program->setModelMatrix(modelMatrix);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		modelMatrix.identity();
		modelMatrix.Translate(x_pos, y_pos, 0.0f);

		if (keys[SDL_SCANCODE_RIGHT] && x_pos + sprite.width / 2 < 1.15f) { x_pos += elapsed * move_speed; } // move right
		else if (keys[SDL_SCANCODE_LEFT] && x_pos - sprite.width / 2 > -1.15f){ x_pos -= elapsed * move_speed; } // move left

		if (keys[SDL_SCANCODE_SPACE]) {
			if (!bullet.alive){
				bullet.x_pos = x_pos;
				bullet.y_pos = y_pos + this->sprite.height * 2.0f;
				bullet.direction = 1.0f;
				bullet.alive = true;
			}

		}
		sprite.Draw();
		bullet.Update(elapsed);
	}
}
