#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(){}
ParticleEmitter::ParticleEmitter(unsigned int particleCount) : maxLifetime(100.0f), triggered(false)
{
	for (int i = 0; i < particleCount; i++){
		particles.push_back(Particle());
	}
}

ParticleEmitter::~ParticleEmitter(){}

void ParticleEmitter::Update(float elapsed){
	if (triggered) {
		for (int i = 0; i < particles.size(); i++) {
			particles[i].x_pos +=  elapsed;
			particles[i].y_pos +=  elapsed;
			particles[i].lifetime += elapsed;
		}
	}
	if (particles[0].lifetime >= maxLifetime)
		Reset();
}

void ParticleEmitter::Render(ShaderProgram *program){
	if (triggered) {
		for (int i = 0; i < particles.size(); i++) {
			vertices.push_back(particles[i].x_pos);
			vertices.push_back(particles[i].y_pos);
		}
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
		glEnableVertexAttribArray(program->positionAttribute);
		glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
	}
}

void ParticleEmitter::Trigger(float x, float y){
	for (int i = 0; i < particles.size(); i++) {
		particles[i].x_pos = x;
		particles[i].x_pos = y;
	}
	triggered = true;
}

void ParticleEmitter::Reset() {
	triggered = false;
	for (int i = 0; i < particles.size(); i++) {
		particles[i].lifetime = 0.0f;
	}
}