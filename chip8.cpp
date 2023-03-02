#include <iostream>
#include <stdio.h>
#include "chip8.h"
using namespace std;

void Chip8::initialize() {
    pc     = 0x200;
    opcode = 0;
    I      = 0;
    sp     = 0;

    // clear display, stack, registers V0-VF, memory
    fill(begin(gfx), end(gfx), 0);
    fill(begin(gfx), end(gfx), 0);
    fill(begin(stack), end(stack), 0);
    fill(begin(V), end(V), 0);
    fill(begin(memory), end(memory), 0);

    // fontset
    for(int i = 0; i < 80; ++i)
        memory[i + 80] = fontset[i];
    
    delayTimer = 0;
    soundTimer = 0;
}

void Chip8::loadGame(char* filePath) {
    FILE* file;
    long lSize;
    char* buffer;
    size_t result;

    file = fopen(filePath, "rb");
    if (file == NULL) {fputs("File error", stderr); exit(1);}

    fseek(file, 0, SEEK_END); // goes until end of file
    lSize = ftell(file); // returns number of bytes since beginning of file
    rewind(file); // rewinds stream to start of file

    buffer = (char*) malloc(sizeof (char) *lSize); // allocates file size amnt of memory for buffer
    if (buffer == NULL) {fputs("Memory error", stderr); exit(2);}

    result = fread(buffer, 1, lSize, file);
    if (result != (unsigned) lSize) {fputs("Reading error", stderr); exit(3);}

    for(int i = 0; i < lSize; ++i)
        memory[i + 512] = buffer[i];
    
    fclose(file);
    free(buffer);
}

void Chip8::emulateCycle() {
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    // decode opcode
    switch(opcode & 0xF000) {
        case 0x0000: // do something
            break;
        case 0x1000: // jump to NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // subroutine @ NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // skip next ins if VX == NN
            (V[opcode & 0x0F00] == (opcode & 0x00FF)) ? pc += 4 : pc += 2;
            break;
        case 0x4000: // skip next ins if VX != NN
            (V[opcode & 0x0F00] != (opcode & 0x00FF)) ? pc += 4 : pc += 2;
            break;
        case 0x5000: // skip next ins if VX == VY
            (V[opcode & 0x0F00] == V[opcode & 0x00F0]) ? pc += 4 : pc += 2;
            break;
        case 0x6000: // set VX to NN
            V[opcode & 0x0F00] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000: // add NN to VX
            V[opcode & 0x0F00] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000: // operations
            handleOperation(opcode & 0x000F);
            pc += 2;
        case 0x9000:
            (V[opcode & 0x0F00] != V[opcode & 0x00F0]) ? pc += 4 : pc += 2;
            break;
        case 0xA000: // Set I to NN
            I = opcode & 0x00FF;
            pc += 2;
            break;
        case 0xB000: // Jump to V0 + NNN
            pc = V[0] + opcode & (0x0FFF);
            break;
        case 0xC000: // Random
            V[opcode & 0x0F00] = rand() & (opcode & 0x0FF);
            pc += 2;
            break;
        case 0xD000: // Draw pixel
            break;
        case 0xE000:
            break;
        case 0xF000:
            break;
        default:
            throw "Invalid instruction encountered";
    }
    // execute opcode

    // update timers
}

void handleOperation(int op) {
    switch (op) {
        case 0x1:
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            break;
        case 0x7:
            break;
        case 0xE:
            break;
        default:
            throw "Invalid instruction encountered"
    }
}