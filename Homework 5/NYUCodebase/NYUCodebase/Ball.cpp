#include "Ball.h"

Ball::Ball(){}
Ball::Ball(float size, float speed) : keys(SDL_GetKeyboardState(NULL)), size(size), speed(speed), x(0.0f), y(0.0f), dir_x(0.0f), dir_y(0.0f) {}

void Ball::Update(float elapsed, Paddle& left, Paddle& right) {
	if (keys[SDL_SCANCODE_SPACE] && dir_x == 0 && dir_y == 0) { dir_x = randDir(); dir_y = randDir(); } // start the ball by hitting the space bar
	x += elapsed * speed * dir_x;
	y += elapsed * speed * dir_y;

	if (y <= -0.975f) { dir_y = 1.0f; } // ball hitting bottom of the screen
	else if (y >= 0.975f) { dir_y = -1.0f; } // ball hitting the top of the screen


	if (-1.652f + left.width / 2 >= x - size / 2 &&
		left.y - left.height / 2 <= y - size / 2 &&
		left.y + left.height / 2 >= y + size / 2) {
		// play sound
		dir_x = 1.0f;
	} // ball hitting left paddle

	if (1.652f - right.width / 2 <= x + size / 2 &&
		right.y - right.height / 2 <= y - size / 2 &&
		right.y + right.height / 2 >= y + size / 2) {
		// play sound
		dir_x = -1.0f;
	} // ball hitting right paddle

	if (x >= 1.752f || x <= -1.752) {
		// play sound
		x = 0.0f; y = 0.0f; dir_x = 0.0f; dir_y = 0.0f;
		left.y = 0.0f;  right.y = 0.0f;
	} // reset if somebody loses
}

void Ball::Render(ShaderProgram* program) {
	program->setModelMatrix(modelMatrix);
	modelMatrix.identity();
	modelMatrix.Translate(x, y, 0.0f);

	float vertices3[] = { -size / 2, -size / 2, size / 2, size / 2, -size / 2, size / 2,
		-size / 2, -size / 2, size / 2, -size / 2, size / 2, size / 2 };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
	glEnableVertexAttribArray(program->positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int Ball::randDir() {
	int val = rand() % 2 + 1;
	if (val == 1) return 1;
	return -1;
}