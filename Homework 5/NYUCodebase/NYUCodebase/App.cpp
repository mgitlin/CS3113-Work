#include "App.h"

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

class App {
public:
	App() : lastFrameTicks(0.0f), angle(0.0f), positionX(0.0f), positionY(0.0f), done(false) {
		Setup();
	}
	~App() {
		delete program;
	}
	void Setup() {
		SDL_Init(SDL_INIT_VIDEO);
		displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
		SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
		SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
		glewInit();
#endif

		glViewport(0, 0, 640, 360);

		projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

		program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	}
	void Update() {
		while (!isDone()) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
				else if (event.type == SDL_KEYDOWN){
					if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
					{
						angle = 0.0f;
					}

				}
				else if (event.type == SDL_MOUSEMOTION){
					float units_x = (((float)event.motion.x / 640.0f) * 3.554f) - 3.55f / 2.0f;
					float units_y = (((float)(360 - event.motion.y) / 360.0f) * 2.0f) - 1.0f;

					positionX = units_x;
					positionY = units_y;
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN){
					angle += 3.14 * .25f;
				}
			}

			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;


			program.setModelMatrix(modelMatrix);
			program.setViewMatrix(viewMatrix);
			program.setProjectionMatrix(projectionMatrix);

			glClear(GL_COLOR_BUFFER_BIT);

			const Uint8 *keys = SDL_GetKeyboardState(NULL);

			/*if (keys[SDL_SCANCODE_LEFT]) {
			angle += elapsed * 2.0f * 3.14f;
			}
			else if (keys[SDL_SCANCODE_RIGHT]){
			angle -= elapsed * 2.0f * 3.14f;
			}*/

			modelMatrix.identity();
			modelMatrix.Translate(positionX, positionY, 0.0f);
			modelMatrix.Rotate(angle);

			glUseProgram(program.programID);

			float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(program.positionAttribute);

			float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);

			glBindTexture(GL_TEXTURE_2D, planeTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
			SDL_GL_SwapWindow(displayWindow);
		}

		SDL_Quit();
	}
	bool isDone() {
		return done;
	}
private:
	SDL_Window* displayWindow;

	ShaderProgram* program;

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	float lastFrameTicks;
	float angle;
	float positionX;
	float positionY;

	bool done;
};