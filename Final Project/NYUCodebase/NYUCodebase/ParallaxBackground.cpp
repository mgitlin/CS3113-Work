#include "ParallaxBackground.h"

ParallaxBackground::ParallaxBackground(){} // Default
ParallaxBackground::ParallaxBackground(GLuint textureID0, GLuint textureID1, GLuint textureID2, GLuint textureID3){
	layers.insert(layers.end(), {
		textureID0,
		textureID1,
		textureID2,
		textureID3
	});
	vertexData.insert(vertexData.end(), {
		-10.0f, 7.5f, // Top Left
		15.5f, -7.5f, // Bottom Right
		-10.0f, -7.5f, // Bottom Left
		-10.0f, 7.5f, // Top Left
		15.5f, 7.5f, // Top Right
		15.5f, -7.5f // Bottom Right
	});
	vector<float> texCoordData0;
	texCoordData0.insert(texCoordData0.end(), {
		0.0f, 0.0f, // Bottom Left
		1.0f, 1.0f, // Top Right
		0.0f, 1.0f, // Top Left
		0.0f, 0.0f, // Bottom Left
		1.0f, 0.0f, // Bottom Right
		1.0f, 1.0f // Top Right
	});
	vector<float> texCoordData1;
	texCoordData1.insert(texCoordData1.end(), {
		0.0f, 0.0f, // Bottom Left
		1.0f, 1.0f, // Top Right
		0.0f, 1.0f, // Top Left
		0.0f, 0.0f, // Bottom Left
		1.0f, 0.0f, // Bottom Right
		1.0f, 1.0f // Top Right
	});
	vector<float> texCoordData2;
	texCoordData2.insert(texCoordData2.end(), {
		0.0f, 0.0f, // Bottom Left
		1.0f, 1.0f, // Top Right
		0.0f, 1.0f, // Top Left
		0.0f, 0.0f, // Bottom Left
		1.0f, 0.0f, // Bottom Right
		1.0f, 1.0f // Top Right
	});
	vector<float> texCoordData3;
	texCoordData3.insert(texCoordData3.end(), {
		0.0f, 0.0f, // Bottom Left
		1.0f, 1.0f, // Top Right
		0.0f, 1.0f, // Top Left
		0.0f, 0.0f, // Bottom Left
		1.0f, 0.0f, // Bottom Right
		1.0f, 1.0f // Top Right
	});
	texCoordData[0] = texCoordData0;
	texCoordData[1] = texCoordData1;
	texCoordData[2] = texCoordData2;
	texCoordData[3] = texCoordData3;
}

void ParallaxBackground::Update(float elapsed){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 12; j += 2){
			texCoordData[i][j] += elapsed * ((i + 1) / 120.0f);
		}
	}
}

void ParallaxBackground::Render(ShaderProgram *program){
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	for (int i = 0; i < 4; i++){
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData[i].data());
		glEnableVertexAttribArray(program->texCoordAttribute);

		glUniform2f(program->scrollUniform, 1.0f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, layers[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 2);
	} 

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}