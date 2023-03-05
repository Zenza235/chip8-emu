#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/gl.
#include "chip8.h"
#include <iostream>
#include <cstdio>
#include <map>
using namespace std;

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int modifier = 10;
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

unsigned char screen_data[SCREEN_WIDTH][SCREEN_HEIGHT][3];
void setupTexture();
void updateTexture(const Chip8 &c8);

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

Chip8 chip8;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No game path specified.");
        return 1;
    }

    if (!chip8.loadGame(argv[1])) {
        printf("Error encountered while loading game.");
        return 1;
    }

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(display_width, display_height, "Chip-8 Emulator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    setupTexture();

    // GLFW loop
    while (!glfwWindowShouldClose(window)) {
        chip8.emulateCycle();

        if(chip8.draw_flag) {
            // clear screen
            glClear(GL_COLOR_BUFFER_BIT);

            // update texture
            updateTexture(chip8);

            // swap buffers
            glfwSwapBuffers(window);

            chip8.draw_flag = false;
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void setupTexture() {
	// Clear screen
	for(int y = 0; y < SCREEN_HEIGHT; ++y)		
		for(int x = 0; x < SCREEN_WIDTH; ++x)
			screen_data[y][x][0] = screen_data[y][x][1] = screen_data[y][x][2] = 0;

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screen_data);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 

	// Enable textures
	glEnable(GL_TEXTURE_2D);
}

void updateTexture(const Chip8 &c8) {	
	// Update pixels
	for(int y = 0; y < 32; ++y)		
		for(int x = 0; x < 64; ++x)
			if(c8.gfx[(y * 64) + x] == 0)
				screen_data[y][x][0] = screen_data[y][x][1] = screen_data[y][x][2] = 0;	// Disabled
			else 
				screen_data[y][x][0] = screen_data[y][x][1] = screen_data[y][x][2] = 255;  // Enabled
		
	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screen_data);

	glBegin( GL_QUADS );
		glTexCoord2d(0.0, 0.0);		glVertex2d(0.0,			  0.0);
		glTexCoord2d(1.0, 0.0); 	glVertex2d(display_width, 0.0);
		glTexCoord2d(1.0, 1.0); 	glVertex2d(display_width, display_height);
		glTexCoord2d(0.0, 1.0); 	glVertex2d(0.0,			  display_height);
	glEnd();
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    if (input_map.find(key) != input_map.end()) {
        int index = input_map.at(key);
        if (action == GLFW_PRESS) {
            chip8.key[index] = 1;
        }
        else if (action == GLFW_RELEASE) {
            chip8.key[index] = 0;
        }
    }
}