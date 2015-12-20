#include "Entity.h"

// Default Constructor
Entity::Entity(){}
// Player Constructor
Entity::Entity(EntityType type, GLint sprite, GLint font, SDL_Scancode leftKey, SDL_Scancode rightKey, float start_x) 
	: keys(SDL_GetKeyboardState(NULL)), sprite(sprite), font(font), leftKey(leftKey), rightKey(rightKey), 
	x_pos(start_x), start_x(start_x), y_pos(-6.0f), score(0.0f), width(104.0f), height(24.0f), type(type) 
{
	textMatrix.identity();
	textMatrix.Translate(x_pos - (width / 48.0f) - 2.0f, -7.0f, 0.0f);
}
// Item Constructor
Entity::Entity(EntityType type, GLint sprite, GLint font, int value, Mix_Chunk* hitSound)
	: keys(SDL_GetKeyboardState(NULL)), sprite(sprite), font(font), value(value), hitSound(hitSound),
	x_pos(rand() % 19 + -9.5 ), y_pos(rand() % 8 + 8), width(40.0f), height(40.0f), type(type) {}

Entity::~Entity(){}

void Entity::Update(float elapsed) {
	if (type == ENTITY_PLAYER) {
		x_pos += elapsed * velocity_x * 5.0f;
		if (keys[leftKey] && x_pos - (width / 48.0f) > (start_x - 5.0f))
			velocity_x = -1.0f;
		else if (keys[rightKey] && x_pos + (width / 48.0f) < (start_x + 5.0f))
			velocity_x = 1.0f;
		else
			velocity_x = 0.0f;
	}
	else if (type == ENTITY_ITEM) {
		y_pos -= elapsed * (rand() % 15 + 3);
		if (y_pos < -8) {
			x_pos = rand() % 19 + -9.5;
			y_pos = rand() % 3 + 8;
		}
	}
	modelMatrix.identity();
	modelMatrix.Translate(x_pos, y_pos, 0.0f);
}

void Entity::FixedUpdate(float fixedElapsed, Entity *other) {
	if (this->type == ENTITY_ITEM && other->type == ENTITY_PLAYER) {
		if (this->y_pos < other->y_pos + (other->height / 48.0f) &&
			this->x_pos + (this->width / 48.0f) > other->x_pos - ((other->width - 50.0f) / 48.0f) &&
			this->x_pos - (this->width / 48.0f) < other->x_pos + ((other->width - 50.0f) / 48.0f)
			) {
			other->score += this->value;
			//Mix_PlayChannel(-1, hitSound, 0);
			x_pos = rand() % 19 + -9.5;
			y_pos = rand() % 3 + 8;
		}		
	}
}

void Entity::Render(ShaderProgram *program){
	program->setModelMatrix(modelMatrix);

	glBindTexture(GL_TEXTURE_2D, sprite);
	
	GLfloat texCoords[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	float aspect = width/height;
	float vertices[] = {
		-0.5f * aspect, -0.5f,
		0.5f * aspect, 0.5f,
		-0.5f * aspect, 0.5f,
		0.5f * aspect, 0.5f,
		-0.5f * aspect, -0.5f,
		0.5f * aspect, -0.5f
	};

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}

void Entity::RenderText(ShaderProgram *program) {
	program->setModelMatrix(textMatrix);
	if (type == ENTITY_PLAYER)
		DrawText(program, font, "Score:" + std::to_string(score), 0.75f, 0.005f);
}

void Entity::DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::reset(){
	if (type == ENTITY_PLAYER) {
		x_pos = start_x;
		score = 0;
	}
	else if (type == ENTITY_ITEM) {
		x_pos = rand() % 19 + -9.5;
		y_pos = rand() % 3 + 8;
	}
}

int Entity::getScore(){
	return score;
}