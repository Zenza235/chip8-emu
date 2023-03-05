#include "chip8.h"
#include <glfw3.h>
#include <iostream>
#include <cstdio>
#include <map>
using namespace std;

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int modifier = 10;
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

Chip8 chip8;

static map<int, int> input_map = {
    {GLFW_KEY_1, 0x1}, 
    {GLFW_KEY_2, 0x2}, 
    {GLFW_KEY_3, 0x3}, 
    {GLFW_KEY_4, 0xC}, 

    {GLFW_KEY_Q, 0x4}, 
    {GLFW_KEY_W, 0x5}, 
    {GLFW_KEY_E, 0x6}, 
    {GLFW_KEY_R, 0xD}, 

    {GLFW_KEY_A, 0x7},
    {GLFW_KEY_S, 0x8}, 
    {GLFW_KEY_D, 0x9}, 
    {GLFW_KEY_F, 0xE}, 

    {GLFW_KEY_Z, 0xA}, 
    {GLFW_KEY_X, 0x0}, 
    {GLFW_KEY_C, 0xB}, 
    {GLFW_KEY_V, 0xF}, 
};

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No game path specified.");
        return 1;
    }

    if (!chip8.loadGame(argv[1])) {
        printf("Error encountered while loading game.");
        return 1;
    }

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(display_width, display_height, "Chip-8 Emulator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    // set up texture

    while (!glfwWindowShouldClose(window)) {
        chip8.emulateCycle();

        if(chip8.draw_flag) {
            // clear screen
            glClear(GL_COLOR_BUFFER_BIT);

            // update texture

            // swap buffers
            glfwSwapBuffers(window);

            chip8.draw_flag = false;
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    
    if (input_map.find(key) != input_map.end()) {
        int index = input_map.at(key);
        if (action == GLFW_PRESS)
            chip8.key[index] = 1;

        else if (action == GLFW_RELEASE)
            chip8.key[index] = 0;
    }
}