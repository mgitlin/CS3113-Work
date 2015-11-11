#include "Level.h"

Level::Level(){};
Level::Level(string fileName, GLuint tileset, ParallaxBackground bg) 
	: tileSet(tileset), bg(bg) {
	ifstream infile(fileName);
	string line;
	if (!infile){
		// Error check 
	}
	while (getline(infile, line)) {
		if (line == "[header]") { // Nothing right now, level data is hard coded
		}
		else if (line == "[layer]") { ReadLayerData(infile); }
	}
	PrepareVertexData();
}

bool Level::ReadLayerData(ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < LEVEL_HEIGHT; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < LEVEL_WIDTH; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					levelData[y][x] = val;
				}
			}
		}
	}
	return true;
}

void Level::PrepareVertexData() {
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] != 0) {
				float u = (float)(((int)levelData[y][x] - 1) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x] - 1) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					TILE_SIZE * x, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u, v,
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});

			}
		}
	}
}

void Level::Update(float elapsed){

}

void Level::FixedUpdate(float fixedElapsed) {
	bg.Update(fixedElapsed);
}

void Level::Render(ShaderProgram* program) {
	program->setModelMatrix(modelMatrix);

	bg.Render(program);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileSet);
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 2);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}