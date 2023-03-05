#pragma once

class Chip8 {
    public:
        Chip8();

        unsigned char gfx[64 * 32]; // 64 * 32 px screen
        unsigned char key[16]; // represents keyboard input

        bool draw_flag;

        void emulateCycle();
        bool loadGame(char* file);

    private:
        unsigned short opcode; // 2 bytes
        unsigned char memory[4096]; // 4096 bytes of ram
        unsigned char V[16]; // registers (V[F] is carry flag)

        unsigned short I; // 12 bits (short is 2 bytes)
        unsigned short pc; // program counter

        // 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        // 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        // 0x200-0xFFF - Program ROM and work RAM


        unsigned char delay_timer;
        unsigned char sound_timer;

        unsigned short stack[16]; // used to handle instruction jumps
        unsigned short sp; // stack pointer

        static unsigned char fontset[80];
        
        void handle0Ins();
        void handle8Ins();
        void drawPixel();
        void handleEIns();
        void handleFIns();
};