#include "ParallaxBackground.h"

ParallaxBackground::ParallaxBackground(){} // Default
ParallaxBackground::ParallaxBackground(GLuint textureID) 
	: textureID(textureID), width(.25f), height(1.0f){ 	
	// Height is 1, Width is .25
	for (int i = 0; i < 4; i++) {
		vertexData.insert(vertexData.end(), {
			-10.0f, 7.5f, // Top Left
			15.5f, -7.5f, // Bottom Right
			-10.0f, -7.5f, // Bottom Left
			-10.0f, 7.5f, // Top Left
			15.5f, 7.5f, // Top Right
			15.5f, -7.5f // Bottom Right
		});

		texCoordData.insert(texCoordData.end(), {
			i * width, 0.0f, // Bottom Left
			(i + 1) * width, height, // Top Right
			i * width, height, // Top Left
			i * width, 0.0f, // Bottom Left
			(i + 1) * width, 0.0f, // Bottom Right
			(i + 1) * width, height // Top Right
		});
	}
}

void ParallaxBackground::Update(float elapsed){
	/*for (int i = 0; i < 4; i++) {
		vertexData[i] = {
			-10.0f, 7.5f, // Top Left
			15.5f, -7.5f, // Bottom Right
			-10.0f, -7.5f, // Bottom Left
			-10.0f, 7.5f, // Top Left
			15.5f, 7.5f, // Top Right
			15.5f, -7.5f // Bottom Right
		};
	}*/
}
void ParallaxBackground::FixedUpdate(){}
void ParallaxBackground::Render(ShaderProgram *program){
	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glUniform2f(program->scrollUniform, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 2);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}