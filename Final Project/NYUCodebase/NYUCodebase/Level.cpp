#include "Level.h"

Level::Level(){}

Level::Level(LevelType type, GLint font, GLint bg, Mix_Music *bgm, vector<Entity> entitiesVec)
	: keys(SDL_GetKeyboardState(NULL)), type(type), font(font), bg(bg), bgm(bgm), complete(false), clock(60.0f)
{
	entities = entitiesVec;
	textMatrix.identity();
	textMatrix.Translate(-9.0f, 7.0f, 0.0f);
	textMatrix2.identity();
	textMatrix2.Translate(-9.0f, 6.0f, 0.0f);
}

Level::~Level(){}

void Level::Update(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
	}
	timeLeftOver = fixedElapsed;
	if (type == LEVEL_FREEPLAY && keys[SDL_SCANCODE_SPACE])
		complete = true;
	if (!complete){
		for (int i = 0; i < entities.size(); i++){
			entities[i].Update(elapsed);
		}
	}
}

void Level::FixedUpdate(){
	if (type == LEVEL_CLOCKED){
		if (clock <= 0.0f){
			if (entities[0].getScore() > entities[1].getScore())
				winner = 0;
			else 
				winner = 1;
			complete = true;
		}
		else
			clock -= (fixedElapsed / 10);
	}
	if (type == LEVEL_SCORED){
		if (entities[0].getScore() >= 25000 || entities[1].getScore() >= 25000){
			if (entities[0].getScore() >= 25000)
				winner = 0;
			else if (entities[1].getScore() >= 25000)
				winner = 1;
			complete = true;
		}
	}
	if (!complete) {
		for (int i = 2; i < entities.size(); i++){ // First 2 are always players
			entities[i].FixedUpdate(fixedElapsed, &entities[0]);
			entities[i].FixedUpdate(fixedElapsed, &entities[1]);
		}
	}
}

void Level::Render(ShaderProgram *program){
	glBindTexture(GL_TEXTURE_2D, bg);
	GLfloat texCoords[] = { 0.0f, 1.0f,	1.0f, 0.0f,	0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 1.0f,	1.0f, 1.0f };
	float vertices[] = { -10.0f, -7.5f, 10.0f, 7.5f, -10.0f, 7.5f, 10.0f, 7.5f, -10.0f, -7.5f, 10.0f, -7.5f };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	for (int i = 0; i < entities.size(); i++){
		entities[i].Render(program);
	}
}

void Level::RenderText(ShaderProgram *program){
	for (int i = 0; i < entities.size(); i++){
		entities[i].RenderText(program);
	}
	program->setModelMatrix(textMatrix);
	if (type == LEVEL_FREEPLAY)
		DrawText(program, font, "Free Play", 1.0f, 0.005f);
	else if (type == LEVEL_CLOCKED) {
		DrawText(program, font, "Against The Clock", 1.0f, 0.005f);
		program->setModelMatrix(textMatrix2);
		DrawText(program, font, "Time Left: " + to_string(static_cast<int>(clock)), 1.0f, 0.005f);
	}
	else if (type == LEVEL_SCORED)
		DrawText(program, font, "First To 25,000", 1.0f, 0.005f);
}

void Level::DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
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

void Level::reset(){
	for (int i = 0; i < entities.size(); i++){ 
		entities[i].reset();
	}
	complete = false;
	clock = 60.0f;
}

bool Level::isComplete(){
	return complete;
}

Mix_Music* Level::getBgm() {
	return bgm;
}

int Level::getWinner() {
	return winner;
}