#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "Matrix.h"

#include <vector>
using namespace std;

class ParallaxBackground {
public:
	ParallaxBackground(); // Default
	ParallaxBackground(GLuint textureID0, GLuint textureID1, GLuint textureID2, GLuint textureID3);

	void Update(float elapsed);
	void Render(ShaderProgram *program);

private:
	/*GLuint layer0;
	GLuint layer1;
	GLuint layer2;
	GLuint layer3;

	float width;
	float height;*/

	vector<GLuint> layers;
	vector<float> vertexData;
	vector<float> texCoordData[4];
};