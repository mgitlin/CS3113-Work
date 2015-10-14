#include "Bullet.h"

Bullet::Bullet() {};
Bullet::Bullet(ShaderProgram* program, SheetSprite sprite) : program(program), sprite(sprite), x_pos(), y_pos(), move_speed(1.5), 
direction(0.0f), alive(false) {};
void Bullet::Update(float elapsed) {
	program->setModelMatrix(modelMatrix);
	modelMatrix.identity();
	modelMatrix.Translate(x_pos, y_pos, 0.0f);
	if (alive) {
		y_pos += elapsed * move_speed * direction;
		sprite.Draw();
	}
	if ((direction == 1 && y_pos > 1.2f) || (direction == -1 && y_pos < -1.2f)) alive = false;
}