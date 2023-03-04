#pragma once

class Chip8 {
    public:
        unsigned char gfx[64 * 32]; // 64 * 32 px screen
        unsigned char key[16]; // represents keyboard input

        void initialize();
        void emulateCycle();
        void loadGame(char* file);
        void setKeys();

    private:
        unsigned short opcode; // 2 bytes
        unsigned char memory[4096]; // 4096 bytes of ram
        unsigned char V[16]; // registers (V[F] is carry flag)

        unsigned short I; // 12 bits (short is 2 bytes)
        unsigned short pc; // program counter

        // 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        // 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        // 0x200-0xFFF - Program ROM and work RAM


        unsigned char delayTimer;
        unsigned char soundTimer;

        unsigned short stack[16]; // used to handle instruction jumps
        unsigned short sp; // stack pointer

        bool drawFlag;

        unsigned char fontset[80] = { 
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
        
        void handle0Ins();
        void handle8Ins();
        void drawPixel();
        void handleEIns();
        void handleFIns();
};