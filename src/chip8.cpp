#include "chip8.h"
#include <iostream>
#include <stdio.h>
using namespace std;

static unsigned char fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    pc     = 0x200;
    opcode = 0;
    I      = 0;
    sp     = 0;

    // clear display, stack, registers V0-VF, memory
    fill(begin(gfx), end(gfx), 0);
    fill(begin(stack), end(stack), 0);
    fill(begin(V), end(V), 0);
    fill(begin(memory), end(memory), 0);

    // fontset
    for(int i = 0; i < 80; ++i)
        memory[i + 80] = fontset[i];
    
    delay_timer = 0;
    sound_timer = 0;

    draw_flag = true;
}

bool Chip8::loadGame(char* filepath) {
    FILE* file;
    long l_size;
    char* buffer;
    size_t result;

    file = fopen(filepath, "rb");
    if (file == NULL) {
        fputs("File error", stderr); 
        return false;
    }

    fseek(file, 0, SEEK_END); // goes until end of file
    l_size = ftell(file); // returns number of bytes since beginning of file
    rewind(file); // rewinds stream to start of file

    buffer = (char*) malloc(sizeof (char) *l_size); // allocates file size amnt of memory for buffer
    if (buffer == NULL) {
        fputs("Memory error", stderr); 
        return false;
    }

    result = fread(buffer, 1, l_size, file);
    if (result != (unsigned) l_size) {
        fputs("Reading error", stderr);
        return false;
    }

    for(int i = 0; i < l_size; ++i)
        memory[i + 512] = buffer[i];
    
    fclose(file);
    free(buffer);

    return true;
}

void setKeys() {

}

void Chip8::emulateCycle() {
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // decode & execute opcode
    switch(opcode & 0xF000) {
        case 0x0000:
            handle0Ins();
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
            pc += (V[opcode & 0x0F00 >> 8] == (opcode & 0x00FF)) ? 4 : 2;
        break;

        case 0x4000: // skip next ins if VX != NN
            pc += (V[opcode & 0x0F00 >> 8] != (opcode & 0x00FF)) ? 4 : 2;
        break;

        case 0x5000: // skip next ins if VX == VY
            pc += (V[opcode & 0x0F00 >> 8] == V[opcode & 0x00F0 >> 4]) ? 4 : 2;
        break;

        case 0x6000: // set VX to NN
            V[opcode & 0x0F00 >> 8] = opcode & 0x00FF;
            pc += 2;
        break;

        case 0x7000: // add NN to VX (no carry)
            V[opcode & 0x0F00 >> 8] += opcode & 0x00FF;
            pc += 2;
        break;

        case 0x8000: // operations
            handle8Ins();
        break;

        case 0x9000: // skip next ins if VX != VY
            pc += (V[opcode & 0x0F00 >> 8] != V[opcode & 0x00F0 >> 4]) ? 4 : 2;
        break;

        case 0xA000: // Set I to NN
            I = opcode & 0x00FF;
            pc += 2;
        break;

        case 0xB000: // Jump to V0 + NNN
            pc = V[0] + opcode & (0x0FFF);
        break;

        case 0xC000: // Random
            V[opcode & 0x0F00 >> 8] = rand() & (opcode & 0x00FF);
            pc += 2;
        break;

        case 0xD000: // Draw pixel
            drawPixel();
        break;

        case 0xE000: // key input
            handleEIns();
        break;

        case 0xF000:
            handleFIns();
        break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    // update timers
    if (delay_timer > 0)
        --delay_timer;
    
    if (sound_timer > 0)
        if (sound_timer == 1)
            printf("BEEP"); // eventually replace w/ sound
}

void Chip8::handle0Ins() {
    switch (opcode & 0x000F) {
        case 0x0000: // clear screen
            fill(begin(gfx), end(gfx), 0);
            draw_flag = true;
            pc += 2;
        break;

        case 0x000E: // return from subroutine
            --sp;
            pc = stack[sp];
            pc += 2;
        break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
    }
}

void Chip8::handle8Ins() {
    unsigned char X = opcode & 0x0F00 >> 8;
    unsigned char Y = opcode & 0x00F0 >> 4;

    switch (opcode & 0x000F) {
        case 0x0000: // assignment
            V[X] = V[Y];
            pc += 2;
        break;

        case 0x0001: // VX | VY
            V[X] |= V[Y];
            pc += 2;
        break;

        case 0x0002: // VX & VY
            V[X] &= V[Y];
            pc += 2;
        break;

        case 0x0003: // VX ^ VY
            V[X] ^= V[Y];
            pc += 2;
        break;

        case 0x0004: // Vx + Vy (VF = 1 if carry else 0)
            V[0xF] = (V[Y] > (0xFF - V[X])) ? 1 : 0;
            V[X] += V[Y];
            pc += 2;
        break;

        case 0x0005: // Vx - Vy (VF = 0 if borrow else 1)
            V[0xF] = (V[Y] > V[X]) ? 0 : 1;
            V[X] -= V[Y];
            pc += 2;
        break;

        case 0x0006: // LSB
            V[0xF] = V[Y] & 0x1;
            V[X] >>= 1;
            pc += 2;
        break;

        case 0x0007: // VX = VY - VX
            V[0xF] = (V[X] > V[Y]) ? 0 : 1;
            V[X] = V[Y] - V[X];
            pc += 2;
        break;

        case 0x000E: // MSB
            V[0xF] = V[X] >> 7;
            V[X] <<= 1;
            pc += 2;
        break;

        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
    }
}

void Chip8::drawPixel() {
    unsigned short vx = V[opcode & 0x0F00 >> 8];
    unsigned short vy = V[opcode & 0x00F0 >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

    V[0xF] = 0;
    for (int yLine = 0; yLine < height; yLine++) {
        pixel = memory[I + yLine];
        for (int xLine = 0; xLine < 8; xLine++) {
            if ((pixel & (0x80 >> xLine)) != 0) {
                if (gfx[(vx + xLine) + ((vy + yLine) * 64)] == 1)
                    V[0xF] = 1;
                gfx[(vx + xLine) + ((vy + yLine) * 64)] ^= 1;
            }
        }
    }
    draw_flag = true;
    pc += 2;
}

void Chip8::handleEIns() {
    unsigned short X = opcode & 0x0F00 >> 8;

    switch (opcode & 0x00FF) {
        case 0x009E: // skip if pressed
            pc += (key[V[X]] != 0) ? 4 : 2;
        break;

        case 0x00A1: // skip if not pressed
            pc += (key[V[X]] == 0) ? 4 : 2;
        break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
    }
}

void Chip8::handleFIns() {
    unsigned char X = opcode & 0x0F00 >> 8;

    switch(opcode & 0x00FF) {
        case 0x0007: // set VX to delay timer
            V[X] = delay_timer;
            pc += 2;
        break;

        case 0x000A: // wait for key press
            bool key_press = false;

            for (int i = 0; i < 16; ++i)
                if (key[i] != 0) {
                    V[X] = i;
                    key_press = true;
                }
            
            if (!key_press)
                return;
            
            pc += 2;
        break;

        case 0x0015: // delay
            delay_timer = V[X];
            pc += 2;
        break;

        case 0x0018: // sound
            sound_timer = V[X];
            pc += 2;
        break;

        case 0x001E: // add VX to I
            I += V[X];
            pc += 2;
        break;

        case 0x0029: // set I to sprite location
            I = V[X] * 0x5;
            pc += 2;
        break;

        case 0x0033: // store decimal vals in mem
            memory[I] = V[X] / 100;
            memory[I + 1] = (V[X] / 10) % 10;
            memory[I + 2] = (V[X] % 100) % 10;
            pc += 2;
        break;

        case 0x0055: // load regs
            for (unsigned short i = 0; i <= X; ++i)
                memory[I + i] = V[i];
            pc += 2;
        break;

        case 0x0065: // write to regs
            for (unsigned short i = 0; i <= X; ++i)
                V[i] = memory[I + i];
            pc += 2;
        break;

        default:
            printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
    }
}