#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "Matrix.h"

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(ShaderProgram* program, unsigned int textureID, float u, float v, float width, float height, float size, GLint wrap_mode)
		: program(program), textureID(textureID), u(u), v(v), width(width), height(height), size(size), wrap_mode(wrap_mode) {};

	void Draw() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
		glBindTexture(GL_TEXTURE_2D, textureID);

		GLfloat texCoords[] = {
			u, v+height,
			u+width, v,
			u, v,
			u+width, v,
			u, v+height,
			u+width, v+height
		};
		
		float aspect = width / height;
		float vertices[] = {
			-0.5f * size * aspect, -0.5f * size,
				0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, 0.5f * size,
				0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, -0.5f * size,
				0.5f * size * aspect, -0.5f * size
		};

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}

	ShaderProgram* program;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float size;
	GLint wrap_mode;
};

class Entity {
public:
	Entity(SheetSprite sprite, float x_pos, float y_pos, float move_speed)
		: sprite(sprite), x_pos(x_pos), y_pos(y_pos), move_speed(move_speed), keys(SDL_GetKeyboardState(NULL)) {};

	const Uint8 *keys;
	SheetSprite sprite;
	float lastFrameTicks;
	float x_pos;
	float y_pos;
	float move_speed;
};

class Bullet : public Entity {
public:

};

class Player : public Entity {
public:
	Player(SheetSprite sprite, float x_pos, float y_pos, float move_speed) 
		: Entity(sprite, x_pos, y_pos, move_speed) {};

	void Update() {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (keys[SDL_SCANCODE_RIGHT] && x_pos + sprite.width / 2 < 1.15f) { x_pos += elapsed * move_speed; } // move right
		else if (keys[SDL_SCANCODE_LEFT] && x_pos - sprite.width / 2 > -1.15f){ x_pos -= elapsed * move_speed; } // move left

		if (keys[SDL_SCANCODE_SPACE]) {
			//shoot
		}
		sprite.Draw();
	}
};

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders In Space!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 450, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glViewport(0, 0, 450, 360); 

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.25, 1.25, -1.0f, 1.0f, -1.0f, 1.0f);

	unsigned int spriteSheet = LoadTexture(RESOURCE_FOLDER"sheet.png");
	
	SheetSprite playerSprite(program, spriteSheet, 211.0f / 1024.0f, 941.0f / 1024.0f, 99.0f / 1024.0f, 75.0f / 1024.0f, 0.2, GL_CLAMP);
	Player player(playerSprite, 0.0f, -0.75f, 1.0f);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program->setModelMatrix(modelMatrix);
		program->setViewMatrix(viewMatrix);
		program->setProjectionMatrix(projectionMatrix);
		glUseProgram(program->programID);

		modelMatrix.identity();
		modelMatrix.Translate(player.x_pos, player.y_pos, 0.0f);

		player.Update();


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}