#include "Enemy.h"

Enemy::Enemy() : x_pos(0.0f), y_pos(0.0f), move_speed(0.0f) {};
Enemy::Enemy(ShaderProgram* program, SheetSprite sprite, SheetSprite shotSprite, float x_pos, float y_pos)
	: program(program), keys(SDL_GetKeyboardState(NULL)), sprite(sprite), shotSprite(shotSprite), x_pos(x_pos), y_pos(y_pos), move_speed(0.5),
	bullet(Bullet(program, shotSprite)), alive(true) {};

void Enemy::Update(const float & player_x_pos, const SheetSprite & player_sprite, Bullet & player_bullet, bool & player_alive, int & enemies_alive) {
	if (alive){
		program->setModelMatrix(modelMatrix);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		modelMatrix.identity();
		modelMatrix.Translate(x_pos, y_pos, 0.0f);
		int trigger = rand() % 50000 + 1;
		if (!bullet.alive && trigger == 1 && player_alive){
			bullet.x_pos = x_pos;
			bullet.y_pos = y_pos - this->sprite.height * 2.0f;
			bullet.direction = -1.0f;
			bullet.alive = true;
		}

		if (y_pos - sprite.height < player_bullet.y_pos + player_bullet.sprite.height &&
			y_pos + sprite.height > player_bullet.y_pos - player_bullet.sprite.height &&
			x_pos + sprite.width > player_bullet.x_pos + player_bullet.sprite.width &&
			x_pos - sprite.width < player_bullet.x_pos - player_bullet.sprite.width 
			){
			alive = false;
			player_bullet.alive = false;
			enemies_alive--;
		}
		if (-0.95f + player_sprite.height > bullet.y_pos - bullet.sprite.height &&
			-0.95f - player_sprite.height < bullet.y_pos + bullet.sprite.height &&
			player_x_pos + player_sprite.width > bullet.x_pos + bullet.sprite.width &&
			player_x_pos - player_sprite.width < bullet.x_pos - bullet.sprite.width
			){
			// rekt also
			bullet.alive = false;
			player_alive = false;
		}

		sprite.Draw();
		bullet.Update(elapsed);
	}
}
