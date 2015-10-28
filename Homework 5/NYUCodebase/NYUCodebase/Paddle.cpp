#include "Paddle.h"

Paddle::Paddle(){}
Paddle::Paddle(float x, float height, float width, float speed, SDL_Scancode upKey, SDL_Scancode downKey)
	: keys(SDL_GetKeyboardState(NULL)), x(x), y(0.0f), height(height), width(width), speed(speed), upKey(upKey), downKey(downKey) {}

void Paddle::Update(float elapsed){
	if (keys[upKey] && y < 0.8f) { y += elapsed; } // move paddle up
	else if (keys[downKey] && y > -0.8f){ y -= elapsed; } // move paddle down
}

void Paddle::Render(ShaderProgram* program){
	program->setModelMatrix(modelMatrix);
	modelMatrix.identity();
	modelMatrix.Translate(x, y, 0.0f);

	float vertices[] = { -width / 2, -height / 2, -width / 2, height / 2, width / 2, height / 2,
		-width / 2, -height / 2, width / 2, -height / 2, width / 2, height / 2 };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}