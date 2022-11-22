#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include "shader.h"
#include "camera.h"
#include "input_manager.h"
#include "chunk_map.h"
#include "chunk_renderer.h"
#include "perlin_generator.h"
#include "resource_manager.h"

int WIDTH = 960;
int HEIGHT = 540;
unsigned TICKS_PER_SECOND = 40;
unsigned FPS = 0;
SDL_Window* window = nullptr;
SDL_GLContext mainContext;

Camera camera;

ChunkMap chunkMap;
ChunkRenderer chunkRenderer(chunkMap);

void initScreen() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Video initialization failed: " << SDL_GetError();
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("Voxel Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    mainContext = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    //SDL_GL_LoadLibrary(NULL);

    //SDL_GL_SetSwapInterval(0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void init() {
    InputManager::mapControl(InputManager::MoveForward, SDL_SCANCODE_W);
    InputManager::mapControl(InputManager::MoveLeft, SDL_SCANCODE_A);
    InputManager::mapControl(InputManager::MoveBackward, SDL_SCANCODE_S);
    InputManager::mapControl(InputManager::MoveRight, SDL_SCANCODE_D);
    InputManager::mapControl(InputManager::MoveUp, SDL_SCANCODE_SPACE);
    InputManager::mapControl(InputManager::MoveDown, SDL_SCANCODE_LSHIFT);


    PerlinGenerator::initialize(123);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            chunkMap.addChunk(new Chunk(i, j));
        }
    }
    ResourceManager::LoadTexture("grass.png", false, "grass");
}

void update() {

}

void render(float delta, Shader& shader) {
    //glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.47f, 0.655f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(camera.getZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 model = glm::f32mat2(1);

    //model = glm::rotate(model, (float)SDL_GetTicks64() / 1000.0f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    shader.Use();
    shader.SetMatrix4("projection", proj);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("model", model);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            chunkRenderer.drawChunk(i, j);
        }
    }

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
    initScreen();
    init();

    Shader shader;
    shader.Compile("default_shader.vert", "default_shader.frag");\

    Uint64 lastTime = SDL_GetTicks64();
    double amountOfTicks = TICKS_PER_SECOND;
    double tps = 1000.0 / amountOfTicks;
    double delta = 0;
    Uint64 timer = SDL_GetTicks64();
    int frames = 0;

    SDL_SetRelativeMouseMode(SDL_TRUE); // trap the mouse to the window

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        double now = SDL_GetTicks64();
        delta += (now - lastTime) / tps;
        double deltaTime = (now - lastTime) / 1000.0f;
        lastTime = now;

        InputManager::update();
        if (InputManager::isKeyDown(InputManager::MoveForward)) {
            camera.ProcessKeyboard(Camera::Forward, deltaTime);
        }
        if (InputManager::isKeyDown(InputManager::MoveLeft)) {
            camera.ProcessKeyboard(Camera::Left, deltaTime);
        }
        if (InputManager::isKeyDown(InputManager::MoveBackward)) {
            camera.ProcessKeyboard(Camera::Backward, deltaTime);
        }
        if (InputManager::isKeyDown(InputManager::MoveRight)) {
            camera.ProcessKeyboard(Camera::Right, deltaTime);
        }
        if (InputManager::isKeyDown(InputManager::MoveUp)) {
            camera.ProcessKeyboard(Camera::Up, deltaTime);
        }
        if (InputManager::isKeyDown(InputManager::MoveDown)) {
            camera.ProcessKeyboard(Camera::Down, deltaTime);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_MOUSEMOTION) {
                camera.processMouseMovement(event.motion.xrel, -event.motion.yrel);
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                }

            }
        }

        while (delta >= 1) {
            update();
            delta--;
        }
        render(static_cast<float>(delta), shader);
        ++frames;
        
        if (SDL_GetTicks64() - timer > 1000) {
            timer += 1000;
            FPS = frames;
            std::cout << "FPS: " << FPS << std::endl;
            frames = 0;
        }
    }
    ResourceManager::Clear();
    SDL_DestroyWindow(window);
    return 0;
}