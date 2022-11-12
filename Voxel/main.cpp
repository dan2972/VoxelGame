#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include "mesh.h"
#include "shader.h"

int WIDTH = 960;
int HEIGHT = 540;
unsigned TICKS_PER_SECOND = 40;
unsigned FPS = 0;
SDL_Window* window = nullptr;
SDL_GLContext mainContext;

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
}

void update() {

}

void render(float delta, Mesh& mesh, Shader& shader) {
    //glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.47f, 0.655f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 model = glm::f32mat2(0.5f);

    model = glm::rotate(model, (float)SDL_GetTicks64() / 1000.0f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    shader.Use();
    shader.SetMatrix4("projection", proj);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("model", model);
    
    mesh.start();
    unsigned int v1 = mesh.addVertex({ -1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f });
    unsigned int v2 = mesh.addVertex({ 1.0f, -1.0f, 1.0f,   0.0f, 1.0f, 0.0f });
    unsigned int v3 = mesh.addVertex({ 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f });
    unsigned int v4 = mesh.addVertex({ -1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f });
    unsigned int v5 = mesh.addVertex({ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f });
    unsigned int v6 = mesh.addVertex({ 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f });
    unsigned int v7 = mesh.addVertex({ 1.0f, 1.0f, -1.0f,   0.0f, 0.0f, 1.0f });
    unsigned int v8 = mesh.addVertex({ -1.0f, 1.0f, -1.0f,  0.0f, 1.0f, 1.0f });
    //front
    mesh.addTriangle({ v1, v2, v3 });
    mesh.addTriangle({ v1, v3, v4 });
    //left
    mesh.addTriangle({ v5, v1, v4 });
    mesh.addTriangle({ v5, v4, v8 });
    //right
    mesh.addTriangle({ v2, v6, v7 });
    mesh.addTriangle({ v2, v7, v3 });
    //back
    mesh.addTriangle({ v6, v5, v8 });
    mesh.addTriangle({ v6, v8, v7 });
    //up
    mesh.addTriangle({ v4, v3, v7 });
    mesh.addTriangle({ v4, v7, v8 });
    //down
    mesh.addTriangle({ v5, v6, v2 });
    mesh.addTriangle({ v5, v2, v1 });
    mesh.end();

    mesh.render();

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
    initScreen();

    Shader shader;
    shader.Compile("default_shader.vert", "default_shader.frag");

    Mesh mesh({ 3, 3 });

    Uint64 lastTime = SDL_GetTicks64();
    double amountOfTicks = TICKS_PER_SECOND;
    double tps = 1000.0 / amountOfTicks;
    double delta = 0;
    Uint64 timer = SDL_GetTicks64();
    int frames = 0;

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        double now = SDL_GetTicks64();
        delta += (now - lastTime) / tps;
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        while (delta >= 1) {
            update();
            delta--;
        }
        render(static_cast<float>(delta), mesh, shader);
        ++frames;
        
        if (SDL_GetTicks64() - timer > 1000) {
            timer += 1000;
            FPS = frames;
            std::cout << "FPS: " << FPS << std::endl;
            frames = 0;
        }
    }
    SDL_DestroyWindow(window);
    return 0;
}