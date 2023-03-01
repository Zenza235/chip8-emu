#include <stdio.h>
using namespace std;

class Chip8 {
    private:
        unsigned short opcode; // 2 bytes
        unsigned char memory[4096]; // 4096 bytes of ram
        unsigned char V[16]; // registers

        unsigned short I; // 12 bits (short is 2 bytes)
        unsigned short pc; // program counter

        // 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        // 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        // 0x200-0xFFF - Program ROM and work RAM

        unsigned char gfx[64 * 32]; // 64 * 32 px screen

        unsigned char delayTimer;
        unsigned char soundTimer;

        unsigned short stack[16]; // used to handle instruction jumps
        unsigned short sp; // stack pointer

        unsigned char key[16]; // represents keyboard input

        unsigned char fontset[80] =
        { 
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
    
    public:
        void initialize();
        void emulateCycle();
        void loadGame(char* file);
        void setKeys();
};

void Chip8::initialize() {
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
    
    delayTimer = 0;
    soundTimer = 0;
}

// https://cplusplus.com/reference/cstdio/fread/
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
    if (result != lSize) {fputs("Reading error", stderr); exit(3);}

    for(int i = 0; i < lSize; ++i)
        memory[i + 512] = buffer[i];
    
    fclose(file);
    free(buffer);
}

void Chip8::emulateCycle() {
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    // decode opcode

    // execute opcode

    // update timers
}