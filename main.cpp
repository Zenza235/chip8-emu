#include <iostream>
#include "chip8.h"

Chip8 chip8;

int main(int argc, char **argv) {
    chip8.initialize();

    std::cout << "completed" << endl;

    return 0;
}