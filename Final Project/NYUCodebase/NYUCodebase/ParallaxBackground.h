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
	ParallaxBackground(GLuint textureID);

	void Update(float elapsed);
	void FixedUpdate();
	void Render(ShaderProgram *program);

private:
	GLuint textureID;

	float width;
	float height;

	vector<float> vertexData;
	vector<float> texCoordData;
};