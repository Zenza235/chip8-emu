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
            handle8();
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
            handleF();
            pc += 2;
            break;
        default:
            throw "Invalid instruction encountered";
    }
    // execute opcode

    // update timers
}

void Chip8::handle8() {
    unsigned short x = opcode & 0x0F00;
    unsigned short y = opcode & 0x00F0;

    switch (opcode & 0x000F) {
        case 0x0000:
            V[x] = V[y];
            break;
        case 0x0001:
            V[x] |= V[y];
            break;
        case 0x0002:
            V[x] &= V[y];
            break;
        case 0x0003:
            V[x] ^= V[y];
            break;
        case 0x0004:
            if (V[y >> 4] > (0xFF - V[x >> 8]))
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[x >> 8] += V[y >> 4];
            break;
        case 0x0005: // VX = VX - VY
            // if ()
            break;
        case 0x0006: // LSB
            V[0xF] = V[x] & 1;
            V[x] >>= 1;
            break;
        case 0x0007: // VX = VY - VX
            break;
        case 0x000E: // MSB
            V[0xF] = V[x] & 128;
            V[x] >>= 1;
            break;
        default:
            throw "Invalid instruction encountered";
    }
}

void Chip8::handleF() {
    unsigned short x = opcode & 0x0F00;

    switch(opcode & 0x00FF) {
        case 0x0007:
            V[x] = delayTimer;
            break;
        case 0x000A: // key operation
            break;
        case 0x0015: // delay
            delayTimer = V[x];
            break;
        case 0x0018: // sound
            soundTimer = V[x];
            break;
        case 0x001E:
            I += V[x];
            break;
        case 0x0029:
            break;
        case 0x0033:
            memory[I] = V[x >> 8] / 100;
            memory[I + 1] = (V[x >> 8] / 10) % 10;
            memory[I + 2] = (V[x >> 8] % 100) % 10;
            break;
        case 0x0055:
            for (unsigned short i = 0; i <= x; ++i)
                memory[I + i] = V[i];
            break;
        case 0x0065:
            for (unsigned short i = 0; i <= x; ++i)
                V[i] = memory[I];
            break;
        default:
            throw "Invalid instruction encountered";
    }
}