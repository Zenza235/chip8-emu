#pragma once

class Chip8 {
    public:
        Chip8();

        unsigned char gfx[64 * 32]; // 2048 px screen
        unsigned char key[16]; // keyboard input
        bool draw_flag;

        bool loadGame(const char* filename);
        void emulateCycle();

    private:
        unsigned short opcode;
        unsigned char memory[4096];
        unsigned char V[16]; // registers (V[F] is carry flag)

        unsigned short I;
        unsigned short pc;

        unsigned char delay_timer;
        unsigned char sound_timer;

        unsigned short stack[16]; // for jumps
        unsigned short sp;

        static unsigned char fontset[80]; // 4x5 px font
        
        void init();

        /* Instruction functions */
        void handle0Ins();
        void handle8Ins();
        void handleDIns();
        void handleEIns();
        void handleFIns();
};