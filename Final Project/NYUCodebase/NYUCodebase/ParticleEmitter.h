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

class Particle {
public:
	float x_pos;
	float y_pos;
	float lifetime;
};

class ParticleEmitter {
public:
	ParticleEmitter();
	ParticleEmitter(unsigned int particleCount);
	~ParticleEmitter();
	void Update(float elapsed);
	void Render(ShaderProgram *program);
	void Trigger(float x, float y);
	void Reset();
	float maxLifetime;
	bool triggered;
	std::vector<Particle> particles;
	std::vector<float> vertices;
};